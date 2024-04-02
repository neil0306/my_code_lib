SELECT *
FROM customers
-- WHERE points > 3000
-- WHERE state = 'va'
-- WHERE state != 'va'  -- 不等于
-- WHERE state <> 'va'  -- 不等于
WHERE birth_date > '1990-01-01';

-- where 语句中可以直接使用【比较运算符】: >, >=, <, <=, =, != (不等于), <> (也是不等于） 
-- 字符串 需要用【单引号】或者【双引号】括起来；注意！！字符串也是不区分大小写的！


-- exercise: Get the orders placed this year
SELECT *
FROM orders
WHERE order_date >= '2019-01-01'
