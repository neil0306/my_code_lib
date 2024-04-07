USE sql_hr;

-- 回顾： self inner jion
SELECT 
	e.employee_id,
    e.first_name,
    m.first_name AS manager
FROM employees e
JOIN employees m
	ON e.reports_to = m.employee_id;  -- 只返回有manager的员工信息
    
-- 用self outer join
SELECT 
	e.employee_id,
    e.first_name,
    m.first_name AS manager
FROM employees e
LEFT JOIN employees m
	ON e.reports_to = m.employee_id;  -- 无论这个员工有没有 report_to 的人（上级），都把join的结果显示出来
    
    