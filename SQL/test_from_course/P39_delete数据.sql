USE sql_invoicing;
DELETE FROM invoices  -- 如果只有这行语句，会将这个表格里的所有数据删除
WHERE invoice_id = 
		(SELECT client_id
		FROM clients
		WHERE name = 'Myworks')