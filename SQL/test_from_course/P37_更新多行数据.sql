USE sql_invoicing;

UPDATE invoices			-- 更新表格中的某一行数据
SET	 payment_total = invoice_total * 0.5,    -- 指定更新数据的列
	payment_date = due_date			
WHERE client_id IN (3,4);		-- 用where来定位修改的哪一行数据
								-- 如果将where语句删掉，就会更新整个表格的那些列，所以一定要小心

-- 注：
-- 使用 MySQL workbench 时，如果想要批量进行修改，需要去 preferences-> SQL Editor -> 取消勾选 safe update.




-- exercise
USE sql_store;

UPDATE customers 
SET points = points + 50
WHERE birth_date < '1990-01-01';
