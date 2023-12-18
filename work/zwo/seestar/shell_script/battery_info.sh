#!/bin/bash

for i in {1..10000}
do
	battery_voltage_mv=`cat /sys/class/power_supply/battery/voltage_now`
	time_now=$(date)
	echo "[$time_now][$i]battery_voltage_now=$battery_voltage_mv"
	sleep 1
done