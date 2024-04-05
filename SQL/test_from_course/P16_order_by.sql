SELECT *
FROM customers
-- ORDER BY first_name;     -- 默认使用表格里的primary key那一列进行排序; ORDER BY 默认按照升序排列
-- ORDER BY first_name DESC;  -- 指定使用降序排列
ORDER BY state DESC, first_name ASC;  -- 先按照 state 进行降序排列，然后具有相同state值的那几行数据再按照 first_name 进行升序排列

SELECT first_name, last_name
FROM customers
ORDER BY birth_date;		-- 在 MySQL 中，允许我们 ORDER BY 和 SELECT 使用完全不同的列 （但是其他数据库很可能不允许）

SELECT first_name, last_name, 10 AS points  -- 我们甚至可以让一个确定的数字赋值到一列里，并给这一列命名
FROM customers
ORDER BY points, first_name;   -- 这个例子中，ORDER BY 使用的是SELECT里指定的列，排序逻辑也是先按照points排序，然后再按照first_name排序

SELECT first_name, last_name, 10 AS points
FROM customers
ORDER BY 1,2;   -- 这里用序号1，2来表示SELECT里选择的列名称，这里1替代了 first_name, 2替代了 last_name (不推荐使用这种方法）


-- exercise: 从order_items 表格中选择 order_id 为2的数据，然后按照商品总价格进行降序排列
SELECT * 		-- order_id, product_id, quantity, unit_price
FROM order_items
WHERE order_id = 2
ORDER BY quantity * unit_price DESC;

-- 下面是一个清晰的结果显示
SELECT *, quantity * unit_price AS total_price 		-- order_id, product_id, quantity, unit_price
FROM order_items
WHERE order_id = 2
ORDER BY total_price DESC;
