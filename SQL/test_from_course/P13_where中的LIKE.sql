SELECT *
FROM customers
-- WHERE last_name  LIKE 'b%'  -- %表示任意字符，数字的个数也是任意的；这里表示筛选last_name中以b开头的每一行
-- WHERE last_name LIKE '%b%'   -- 过滤last_name中包含字母b的数据。 说明%摆放的位置是任意的 
WHERE last_name LIKE '_____y'  -- 过滤last_name的长度为6个字符，并且第6个字符是y的数据；这里的下划线相当于一个占位符，它可以表示任何东西
;

-- % 用来代表任何字符（不管是什么字符）
-- _ 用来代表单个字符（不管是什么字符）


-- exercise
SELECT * 
FROM customers
-- WHERE address LIKE "%trail%" OR address LIKE '%avenue%'   -- 练习1
WHERE phone LIKE '%9' -- 练习2
