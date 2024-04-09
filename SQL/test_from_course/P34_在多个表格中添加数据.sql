-- 往多个表格添加数据

INSERT INTO orders(customer_id, order_date, status)
VALUES (1, '2019-01-02', 1);



-- 用MySQL内置的函数 "LAST_INSERT_ID" 可以寻找当前表格里最近插入的ID
-- SELECT LAST_INSERT_ID() -- 可以获得上面INSERT INTO之后的那个 order_id （这是 auto-increase 属性自动生成的值，用这个函数可以拿到最后一次生成的数值）
INSERT INTO order_items
VALUES (LAST_INSERT_ID(), 1, 1, 2.95),
	(LAST_INSERT_ID(), 2, 1, 3.95);

