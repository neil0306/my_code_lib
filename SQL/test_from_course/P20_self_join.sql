USE sql_hr;

SELECT 
	e.employee_id,
    e.first_name,
    m.first_name AS manager 
FROM employees e   -- 在self-join的时候，为了不产生歧义，一般都需要给这个表格建立别名，并在整个query语句中都使用这个别名
JOIN employees m 
	ON e.reports_to = m.employee_id

-- self join 主要需要注意的就是这个表格至少得赋予一次别名，否则会造成索引歧义，导致报错。