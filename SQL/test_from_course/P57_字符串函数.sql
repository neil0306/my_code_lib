-- 字符串处理函数：
	# LENGTH() 求字符串长度
    # UPPER() 转大写
    # LOWER() 转小写
    # LTRIM() 去除字符串左侧的空格
		# 这个函数是 left trim 的缩写
	# RTRIM() 去除字符串右侧空格
    # TRIM() 去除左侧和右侧空格
    # LEFT() 返回字符串左侧n个字符
		# LEFT("what the fuck",6)  -> "what t"
	# RIGHT() 同 LEFT，返回右侧n个字符
    # SUBSTRING() 返回子串
		# SUBSTRING("Kindergarten", 3, 5) -> 'nderg'； 从第3个字符串开始，一共取出5个字符出来
        # SUBSTRING("Kindergarten", 3) -> "ndergarten"; 如果不指定长度，则返回从指定的起始字符到末尾的字符串
	# LOCATE() 检索字符串中某个子串的【第一次出现的起始位置】
		# LOCATE("n","Kindergarten") -> 返回3， 因为n首次出现在"Kindergarten"是在第三个字母的位置
        # 如果相差找的字符不存在，函数会返回0
	# REPLACE() 替换字符串中的子串，后续如果还有字符时，这些字符会被保留
		# REPLACE("Kindergarten", 'garten', 'garden1111')  -> "Kindergarden1111"; 先找到 garten, 然后用 garden进行了替换
        # REPLACE("Kindergartennnnnnnnnnnnn", 'garten', 'garden1111')  -> 'Kindergarden1111023456'; 先找到 garten，将这部分子串替换为garden1111，后面的字符保持不变。
	# CONCAT() 拼接字符串


-- 要查找更多字符串处理函数，可以 Google 搜 mysql string function

SELECT TRIM('  sky   ');
SELECT LEFT("what the fuck",6);
SELECT SUBSTRING("Kindergarten", 3, 5);
SELECT LOCATE("n","Kindergarten");
SELECT LOCATE("x","Kindergarten");  -- 返回0，因为x不在 "Kindergarten" 中出现
SELECT REPLACE("Kindergarten", 'garten', 'garden1111');
SELECT REPLACE("Kindergarten023456", 'garten', 'garden1111');

USE sql_store;	
SELECT concat(first_name, " ", last_name) AS full_name		-- 将两列数据拼接到一起了
FROM customers;