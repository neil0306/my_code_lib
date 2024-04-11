-- 数值处理相关函数
	# ROUND() 用来进行四舍五入
		# ROUND (5.7345) -> 6
        # ROUDN (5.7345,1) -> 5.7   指定保留1位小数，第二位的小数将进行四舍五入
	# TRUNCATE () 用来直接截断数值
		# TRUNCATE (5.2734, 1)  -> 5.2  只保留1位小数，7没有进行四舍五入进位
	# CEILING() 向上取整
		# CEILING (5.1) -> 6   返回【大于等于当前数值】的整数
	# FLOOR()  向下取整
		# FLOOR (5.9)  -> 5    返回【小于等于当前数值】的整数
	# ABS()  取绝对值
	# RAND() 取[0,1]之间的随机数

-- 更多数值处理函数可以google搜： mysql numeric functions
    
    
SELECT FLOOR(5.973)