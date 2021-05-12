# translator.py

from math import ceil, floor


def determine_mode():
    if params["mode_target"] == "OFF":
        return 1

    if params["mode_target"] == "HEAT":
        if params["current_humidity"] - params["humidity_target"] > 5:
            return 2
        else: 
            return 1

    if params["mode_target"] == "COOL":
        if params["current_temp"] - params["temp_target"] > 1:
            return 8
        elif params["current_humidity"] - params["humidity_target"] > 5:
            return 2
        else:
            return 1
        
    if params["mode_target"] == "AUTO":
        if params["current_temp"] - params["cool_threshold"] > 1:
            return 8
        elif params["current_humidity"] - params["humidity_target"] > 5:
            return 2
        else:
            return 1
        

def determine_power():
    if msg["payload"]["mode"] in [2, 8]:
        return True
    elif params["fan_power"]:
        return True
    else:
        return False


def determine_fan():
    if msg["payload"]["mode"] == 1:
        if 0 <= params["fan_speed"] <= 33:
            return 4
        elif 33 < params["fan_speed"] <= 66:
            return 2
        elif 66 < params["fan_speed"] <= 100:
            return 1

    else:
        if params["current_humidity"] - params["humidity_target"] > 15 or params["current_temp"] - params["temp_target"] > 3:
            return 1
        elif params["current_humidity"] - params["humidity_target"] > 10 or params["current_temp"] - params["temp_target"] > 2:
            return 2
        else:
            return 4


params = msg["payload"]
msg["payload"] = {}

msg["payload"]["unit_f"] = True if params["temp_unit"] == "FAHRENHEIT" else False
msg["payload"]["mode"] = determine_mode()
msg["payload"]["temp"] = floor(params["temp_target"] - 1)
msg["payload"]["power"] = determine_power()
msg["payload"]["fan"] = determine_fan()
msg["payload"]["timer"] = False
msg["payload"]["timer_val"] = 1


return msg