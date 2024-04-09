# WITH ROLLUP 这个运算符只在 MySQL 中能用，其他数据库比如 orical 之类的数据库没有这个运算符，但是有其他相似的运算符进行替代

USE sql_invoicing;

SELECT 
	client_id,
    SUM(invoice_total) AS total_sales
FROM invoices
GROUP BY client_id WITH ROLLUP;  # ROLLUP 将GROUPBY 之后的值加起来


-- case: GROUP BY 多列数据
SELECT 
	state,
    city,
    SUM(invoice_total) AS total_sales
FROM invoices i
JOIN clients c
	USING (client_id)
GROUP BY state, city WITH ROLLUP;  # ROLLUP 在这里将每一个组进行了求和
									## 如：对【每一个state里】的【所有city分组里的数据】进行求和
                                    ## 	  对所有city的数据进行求和

-- exercise
SELECT 
	pm.name AS payment_method,
    SUM(amount) AS total
FROM payments p
JOIN payment_methods pm
	ON p.payment_method = pm.payment_method_id
GROUP BY pm.name WITH ROLLUP   ## 在使用ROLLUP的时候，不能使用 SELECT 里起的别名， 否则会报错
