#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <OpenXLSX.hpp>
#include <iostream>

class AsyncExcelWriter {
private:
    std::queue<std::vector<double>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::thread m_worker;
    bool m_stop = false;

    OpenXLSX::XLDocument m_doc;
    OpenXLSX::XLWorksheet m_wks;
    std::string m_filename;
    std::vector<std::string> m_headers;
    int m_row = 2; // первая строка — заголовки, вторая — начало данных

    void loop() {
        while (true) {
            std::vector<double> data;

            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_cond.wait(lock, [this] { return m_stop || !m_queue.empty(); });

                if (m_stop && m_queue.empty()) {
                    break;
                }

                if (!m_queue.empty()) {
                    data = std::move(m_queue.front());
                    m_queue.pop();
                } else {
                    continue;
                }
            }

            if (data.size() != m_headers.size()) {
                std::cerr << "Warning: data size mismatch! Expected " << m_headers.size()
                << ", got " << data.size() << std::endl;
                continue;
            }

            for (size_t i = 0; i < data.size(); ++i) {
                m_wks.cell(OpenXLSX::XLCellReference(m_row, static_cast<uint16_t>(i + 1))).value() = data[i];
            }

            m_row++;
            m_doc.save();
        }
    }

public:
    AsyncExcelWriter(const std::string& filename, const std::vector<std::string>& headers)
        : m_filename(filename), m_headers(headers) {

        try {
            m_doc.create(m_filename);
            m_wks = m_doc.workbook().worksheet("Sheet1");

            for (size_t i = 0; i < m_headers.size(); ++i) {
                m_wks.cell(OpenXLSX::XLCellReference(1, static_cast<uint16_t>(i + 1))).value() = m_headers[i];
            }

            m_doc.save();

            m_worker = std::thread(&AsyncExcelWriter::loop, this);

        } catch (const std::exception& e) {
            std::cerr << "Failed to create Excel file: " << e.what() << std::endl;
            throw;
        }
    }

    ~AsyncExcelWriter() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stop = true;
        }
        m_cond.notify_all();
        if (m_worker.joinable()) {
            m_worker.join();
        }

        if (m_doc.isOpen()) {
            m_doc.save();
            m_doc.close();
        }
    }

    template<typename Container>
    void addToQueue(Container&& data) {
        static_assert(std::is_same_v<std::decay_t<typename Container::value_type>, double>,
                      "Container must contain values convertible to double");

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.emplace(std::begin(data), std::end(data));
        }
        m_cond.notify_one();
    }

    AsyncExcelWriter(const AsyncExcelWriter&) = delete;
    AsyncExcelWriter& operator=(const AsyncExcelWriter&) = delete;
};
