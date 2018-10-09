from Parser import Parser
from SimulateMotion import SimulateMotion

import sys

""" What do we want from Simulator:
- Divide into three clearly marked phases,
    1) Zeroing and traveling to staging area
    2) Picking and proceeding to drop location
    3) Droping and returning to staging
- Should be able to generically place arrows 
    at the midpoint of each trajectory line 
    indicating the direciton of travel.
- Overlay PC_STATUS on the simulated image.
- Create a static opaque box that represents the 
    RPC bin based on the specs passed from the config.
- BONUS: adjust scaling of axis to more accurately
    represent the robot, based on the parameters passed 
    from the config.
"""

if __name__ == "__main__":
    parser = Parser()
    parser.text_file_to_dataframe("../../Data/positionData.txt")
    phase_map = parser.split_into_phases(parser.df_map)

    json = parser.load_json("../../extras/configurations/small_machine_config.json")

    SimulateMotion(phase_map, parser.status_locations, json).modelData()