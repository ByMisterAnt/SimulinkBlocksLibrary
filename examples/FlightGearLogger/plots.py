import pandas as pd
import matplotlib.pyplot as plt
import os

EXCEL_FILE = "flight_data_elevator.xlsx" # Имя файла
SHEET_NAME = "Sheet1"                    # Имя листа
TIME_COLUMN = "cur_time"                 # Имя колонки со временем
OUTPUT_DIR = "plots"                     # Папка для сохранения графиков
SHOW_PLOTS = True                        # Показывать ли графики на экране
SAVE_PLOTS = True                        # Сохранять ли графики в файлы

# Создание папки для графиков
if SAVE_PLOTS and not os.path.exists(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)

# Загрузка данных
print(f"Загрузка данных из {EXCEL_FILE}...")
df = pd.read_excel(EXCEL_FILE, sheet_name=SHEET_NAME)

if TIME_COLUMN not in df.columns:
    raise ValueError(f"Колонка времени '{TIME_COLUMN}' не найдена. Доступные колонки: {list(df.columns)}")

time_data = df[TIME_COLUMN]

# Построение графиков
print("Построение графиков...")

for column in df.columns:
    if column == TIME_COLUMN:
        continue  # Пропускаем колонку времени

    plt.figure(figsize=(12, 6))
    plt.plot(time_data, df[column], label=column, linewidth=2)
    plt.title(f"{column} vs Time", fontsize=16)
    plt.xlabel("Time (s)", fontsize=12)
    plt.ylabel(column, fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()

    if SAVE_PLOTS:
        safe_name = "".join(c if c.isalnum() or c in "._-" else "_" for c in column)
        filename = os.path.join(OUTPUT_DIR, f"{safe_name}.png")
        plt.savefig(filename, dpi=150)
        print(f" → Сохранён график: {filename}")

    if SHOW_PLOTS:
        plt.show()
    else:
        plt.close()

# Опционально: все параметры на одном графике (нормализованные)
plt.figure(figsize=(14, 8))

for column in df.columns:
    if column == TIME_COLUMN:
        continue
    # Нормализуем для визуального сравнения (опционально)
    series = df[column]
    normalized = (series - series.min()) / (series.max() - series.min())
    plt.plot(time_data, normalized, label=column)

plt.title("All Parameters (Normalized) vs Time", fontsize=16)
plt.xlabel("Time (s)", fontsize=12)
plt.ylabel("Normalized Value", fontsize=12)
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
plt.tight_layout()

if SAVE_PLOTS:
    plt.savefig(os.path.join(OUTPUT_DIR, "all_parameters_normalized.png"), dpi=150)
if SHOW_PLOTS:
    plt.show()

print("Готово! Все графики построены.")
