USE sql_invoicing;

UPDATE invoices			-- 更新表格中的某一行数据
SET	 payment_total = invoice_total * 0.5,    -- 指定更新数据的列
	payment_date = due_date			
WHERE invoice_id = 3		-- 用where来定位修改的哪一行数据



