# 日期格式化
SELECT DATE_FORMAT(NOW(), "%m %d %y");  # 小写的y，返回年份（2位数字）；大写的Y，返回完整的4位数(如2024)
SELECT DATE_FORMAT(NOW(), "%M %D %Y");  # 大写的M和D，会返回英文单词版本的日期；如 'April 11th 2024'

-- 可以 google 搜 mysql date format string

SELECT DATE_FORMAT(NOW(), "%H:%I %p");  # H 表示24小时制的小时，i表示分钟，p表示am/pm (只能用小写）； 如 '14:02 PM'
SELECT DATE_FORMAT(NOW(), "%h:%i %p");  # 小写h表示12小时制的小时，I也还是分钟，p还是小写。 如'02:03 PM'

