# "GP" - информация получена от спутников GPS (США).
# "GL" - информация получена от спутников Глонасс (Россия).
# "GA" - информация получена от спутников Galileo (Европа).
# "BD" - информация получена от спутников Beidou (Китай).
# "GQ" - информация получена от спутников QZSS (Япония).
# "GN" - информация получена от спутников разных навигационных систем.

# "GGA" - Данные о последнем зафиксированном местоположении.
# "GLL" - Географические координаты.
# "GSA" - Информация об активных спутниках (участвующих в позиционировании).
# "GSV" - Информация о всех наблюдаемых спутниках.
# "RMC" - Рекомендуемый минимум навигационных данных.
# "VTG" - Скорость и курс относительно земли.
# "ZDA" - Дата и время.
# "DHV" - Информация о скорости движения GNSS приемника.
# "GST" - Статистика ошибок позиционирования.
# "TXT" - Текстовое сообщение.


from serial import Serial
from pynmeagps import NMEAReader


stream = Serial('/dev/ttyUSB0', 115200, timeout=3)
nmr = NMEAReader(stream)


def filter(fil_value, value):
    if fil_value == 0:
        return value
    return 0.8 * fil_value + 0.2 * value


fil_lat, fil_lon = 0, 0

lat_min, lat_max = 1000, -1000
lon_min, lon_max = 1000, -1000
i = 0

output = []

for (raw_data, parsed_data) in nmr:

    if parsed_data.msgID == "RMC":
        i += 1
        # print(str(raw_data)[2:-5])
        lat, lon = parsed_data.lat, parsed_data.lon

        fil_lat = filter(fil_lat, lat)
        fil_lon = filter(fil_lon, lon)

        if lat < lat_min:
            lat_min = lat
        if lat > lat_max:
            lat_max = lat

        if lon < lon_min:
            lon_min = lon
        if lon > lon_max:
            lon_max = lon

        print(lat, lon)

print("################")
print(lat_min, lon_min)
print(lat_max, lon_max)

print(lat_max, lon_min)
print(lat_min, lon_max)
