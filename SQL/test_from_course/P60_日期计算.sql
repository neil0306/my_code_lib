# 日期计算
## date_add
SELECT DATE_ADD(NOW(), INTERVAL 1 DAY);  -- 计算后1天的时间，（时分秒不变，只改变【天】）
SELECT DATE_ADD(NOW(), INTERVAL -1 DAY); -- 计算前1天的时间，（时分秒不变，只改变【天】）
SELECT DATE_ADD(NOW(), INTERVAL 1 YEAR); -- 计算后一年
SELECT DATE_ADD(NOW(), INTERVAL -1 YEAR); -- 计算前一年

## date_sub
SELECT DATE_SUB(NOW(), INTERVAL -1 DAY);  # 结果与前面差不多，但是要注意数值的正负号


## datediff
SELECT DATEDIFF("2019-01-05 09:00", "2019-01-01 17:00");  # 计算相差多少【天】, 忽略时分秒；返回结果有正数和负数

## TIEM_TO_SEC 计算从00:00点开始的【秒数换算】
SELECT time_to_sec('09:00');   # 返回32400；  9*60*60=32400
SELECT time_to_sec('09:00') - time_to_sec('09:02');  # 返回 -120
