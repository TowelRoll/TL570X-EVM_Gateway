#!/bin/sh
###
 # @Description: 
 # @Version: 1.0
 # @Autor: Towel Roll
 # @Date: 2022-03-14 11:35:20
 # @LastEditors: Towel Roll
 # @LastEditTime: 2022-03-14 13:24:32
### 
echo -e "Content-type: text/plain\n"

#REQUEST_METHOD="POST"#测试用
#query_string="GETVER"
# echo $REQUEST_METHOD

version=`dpkg -l | grep media-automount-deb | grep -Eo "[0-9]*+\.*[0-9]+\.*[0-9]"`
new_ver=`dpkg --info /www/cgi-bin/media-automount*.deb | grep Version | grep -Eo "[0-9]*+\.*[0-9]+\.*[0-9]"`

if [ "$REQUEST_METHOD" = "POST" ]; then
	#cat 接收post
	query_string=`cat`
	# echo $query_string
	if [ "$query_string" = "GETVER" ]; then
		echo "$version"
		exit 0
	elif [ "$query_string" = "UPDATE" ]; then
		# echo "$new_ver"
		if [ -z "$new_ver" ]; then
			echo "更新失败：更新文件不存在，请上传后重新点击"
		# elif [ "$new_ver" = "$version" ]; then
		# 	echo "更新失败：当前版本为最新版本，无需更新"
		else # elif [ "$new_ver" \> "$version" ]; then
			if sudo dpkg -i /www/cgi-bin/media-automount_1.0.0_armhf.deb
			then
				echo "更新成功!"	
			else
			   	echo "更新失败：请重新更新！错误码：$?"
			fi		
		# elif [ "$new_ver" \< "$version" ]; then
		# 	echo "更新失败：不能更新为旧版本!"		
		fi
		exit 0
	fi
fi

echo "error"
