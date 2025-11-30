import pandas as pd

# 1. 定义列名 (根据 Features.txt)
columns = [
    "Lever_pos", "Ship_speed", "GTT_torque", "GT_rpm", "GG_rpm", 
    "Starboard_Prop_T", "Port_Prop_T", "HP_T_exit_temp", "GT_Comp_in_temp", 
    "GT_Comp_out_temp", "HP_T_exit_press", "GT_Comp_in_press", 
    "GT_Comp_out_press", "GT_exhaust_press", "TIC_control", "Fuel_flow",
    "Target_Compressor_Decay", "Target_Turbine_Decay" # 两个目标值
]

# 2. 读取原始数据
df = pd.read_csv('data.txt', sep='\s+', header=None, names=columns)

# 3. 保存为 CSV (Edge Impulse 可识别)
df.to_csv('naval_propulsion_dataset.csv', index=False)
print("转换完成: naval_propulsion_dataset.csv")