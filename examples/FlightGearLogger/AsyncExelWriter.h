#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <OpenXLSX.hpp>
#include <iostream>
#include <cassert>

class AsyncExcelWriter {
private:
    static constexpr uint32_t BATCH_SIZE = 100;  // Настройте по усмотрению

    struct Batch {
        std::vector<std::vector<double>> rows;
        int start_row = 0;

        Batch() = default;
        Batch(int sr) : start_row(sr) {}
    };

    std::queue<std::vector<double>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::thread m_worker;
    bool m_stop = false;

    OpenXLSX::XLDocument m_doc;
    OpenXLSX::XLWorksheet m_wks;
    std::string m_filename;
    std::vector<std::string> m_headers;
    int m_row = 2; // строка 1 — заголовки

    void loop() {
        while (true) {
            std::vector<std::vector<double>> batch_data;
            int batch_start_row = 0;

            {
                std::unique_lock<std::mutex> lock(m_mutex);

                // Ждём, пока либо есть BATCH_SIZE строк, либо завершение + остатки
                m_cond.wait(lock, [this] {
                    return m_stop || m_queue.size() >= BATCH_SIZE;
                });

                if (m_queue.empty() && m_stop) {
                    break;
                }

                // Забираем до BATCH_SIZE строк
                batch_start_row = m_row;
                size_t take = std::min(static_cast<size_t>(BATCH_SIZE), m_queue.size());
                batch_data.reserve(take);

                for (size_t i = 0; i < take; ++i) {
                    assert(!m_queue.empty());
                    batch_data.emplace_back(std::move(m_queue.front()));
                    m_queue.pop();
                    ++m_row;
                }

                // Если данные остались — разбудим себя снова при след. addToQueue
                if (!m_queue.empty() && !m_stop) {
                    m_cond.notify_one(); // необязательно, но помогает при частых мелких добавлениях
                }
            }

            // Пишем батч в Excel (без блокировки мьютекса!)
            if (!batch_data.empty()) {
                writeBatch(batch_start_row, batch_data);
                m_doc.save();  // сохраняем один раз на батч
            }
        }

        // Финальный flush оставшихся данных (< BATCH_SIZE)
        flushRemaining();
    }

    void writeBatch(int start_row, const std::vector<std::vector<double>>& batch) {
        assert(!batch.empty());
        size_t cols = m_headers.size();

        for (size_t r = 0; r < batch.size(); ++r) {
            const auto& row = batch[r];
            if (row.size() != cols) {
                std::cerr << "Warning: row size mismatch! Expected " << cols
                          << ", got " << row.size() << " at row " << (start_row + r) << std::endl;
                continue;
            }

            for (size_t c = 0; c < cols; ++c) {
                m_wks.cell(OpenXLSX::XLCellReference(start_row + static_cast<int>(r), static_cast<uint16_t>(c + 1)))
                .value() = row[c];
            }
        }
    }

    void flushRemaining() {
        std::vector<std::vector<double>> remaining;
        int start_row = 0;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_queue.empty()) return;
            start_row = m_row;
            remaining.reserve(m_queue.size());
            while (!m_queue.empty()) {
                remaining.emplace_back(std::move(m_queue.front()));
                m_queue.pop();
                ++m_row;
            }
        }

        if (!remaining.empty()) {
            writeBatch(start_row, remaining);
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
            std::cerr << "Failed to create Excel file '" << filename << "': " << e.what() << std::endl;
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
            m_worker.join();  // join() вызовет flushRemaining() внутри loop
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

    // Запрет копирования
    AsyncExcelWriter(const AsyncExcelWriter&) = delete;
    AsyncExcelWriter& operator=(const AsyncExcelWriter&) = delete;
};
