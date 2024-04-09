USE sql_store;

CREATE TABLE orders_archived AS 
SELECT * FROM orders;   -- sub-query 语句


-- 注意：
-- 用 CREATE TABLE xxx AS 语句快速复制一张表格时，生成的表格【不会有primary key，同时不会保持原来表格的 AI，NN 等属性】
-- 【另】使用 MySQL workbench 右键菜单里的 [truncate table] 功能可以清空整个表格里的数据

-- 利用 sub-query 往表格中插入数据
INSERT INTO orders_archived    -- 不使用圆括号时，表示对表格的每一列都插入数据
SELECT *
FROM orders
WHERE order_date < '2019-01-01';

-- exercise
USE sql_invoicing;

CREATE TABLE invoice_archived AS   -- 快速创建表格 invoice_archived
SELECT                            -- 使用 sub-query 语句定制表格中的内容	
	i.invoice_id,
    i.number,
    c.name AS client,
    i.invoice_total,
    i.payment_total,
    i.invoice_date,
    i.due_date,
    i.payment_date
FROM invoices i
JOIN clients  c
	ON i.client_id = c.client_id  -- 还可以用 USING (client_id)
WHERE i.payment_date is NOT NULL;

