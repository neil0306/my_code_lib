# now， curdate，curtime
SELECT NOW(), CURDATE(), CURTIME();    -- 2024-04-11 13:23:46', '2024-04-11', '13:23:46'
										# NOW： 返回当前完整时间
										# CURDATE：返回日期
                                        # CURTIME：返回时间

# 截取某些日期字段
## 下面的函数返回的是数值类型
SELECT YEAR(NOW());	   	# 年 2024 ；只截取了年份
SELECT MONTH(NOW());  	# 月
SELECT DAY(NOW());		# 日
SELECT HOUR(NOW());		# 时
SELECT MINUTE(NOW());	# 分
SELECT SECOND(NOW());	# 秒

## 下面的函数返回的是字符型
SELECT MONTHNAME(NOW());  	# 月
SELECT DAYNAME(NOW());		# 日

# 标准SQL的内置函数 extract，所有Database都会有这个函数
   -- 希望代码可以跨数据库管理器执行的话，尽可能使用extract函数
SELECT EXTRACT(YEAR FROM NOW());    # extract(时间单位 from 可以获取到时间的函数)
SELECT EXTRACT(MONTH FROM NOW()); 
SELECT EXTRACT(DAY FROM NOW()); 




-- exercise
USE sql_store;

SELECT *
FROM orders
WHERE YEAR(order_date) = YEAR(NOW());  # 获取当年的订单
