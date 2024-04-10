-- Select employees whose salary is above the average in their office

USE sql_hr;

-- 思路（伪代码）
# for each employess
	# calculate the avg salary for employee.office
    # return the employee if salary > avg 

SELECT *
FROM employees e
WHERE salary > (
	SELECT AVG(salary)
    FROM employees
    WHERE office_id = e.office_id			# 这句话是关键，称为【correlated sub-query】，特点是sub-query里【引用了外层查询里的表格（或别名）】; 
											# 执行时，会对面的 e 表格的每一行数据都执行一次这里的sub-query, 
                                            # 	即：针对每一个员工，先计算【与他同一个部门】的人员的平均工资，然后看看当前这个员工的工资是否大于这个部门的平均工资（外层的where语句），
                                            #       如果大于平均工资，则这个员工的信息将显示到最终结果里。
);

-- 在correlated sub-query 中，sub-query 会被执行很多次（外层表格如果有N条数据，那么这个sub-query就会被执行N次）；
	-- 因此，correlated sub-query 的执行通常很慢，而且耗内存。 （但由于它的功能还是挺强的，所以依旧有使用场景）
-- 而非 correlated sub-query 中，sub-query只会执行一次。



-- exercise: get invoices that are larger than the client's average invoice amount.
			# 针对每一个客户，返回 【大于该客户平均发票金额】的所有发票
USE sql_invoicing;

SELECT * 
FROM invoices i
WHERE invoice_total > (
	SELECT 
		AVG(invoice_total)
	FROM invoices
    WHERE i.client_id = client_id   # correlated sub-query
);
 
