import pandas as pd
import re
import json

from CommonIncludes import PC_STATUS

class Parser:

    def __init__(self):
        self._columns = ['X', 'Y', 'Z']
        self._num_items_picked = 0
        self._status = "Status state: "
        self._new_session = "NEW SESSION"
        self._realtime = "REALTIME"
        self._simulation = "SIMULATED"
        self._session_num = -1

        self.status_locations = {}
        self._df = pd.DataFrame(columns=self._columns)
        self.df_map = {}

    def text_file_to_dataframe(self, filepath):
        with open(filepath) as file:
            data = file.read().splitlines()

        coord_row = 0
        status_map = {}
        row_values = pd.DataFrame([[0, 0, 0]], columns=self._columns) # Stubbed for simulation purposes
        for row in data:
            if self._new_session in row:
                self._df = pd.DataFrame(columns=self._columns)
                status_map.clear()
                coord_row = 0
                self._session_num += 1
            elif self._realtime or self._simulation in row:
                continue
            elif self._status in row:
                self._pc_status = self._determine_status(row.split(self._status, 1)[1])
                status_map.setdefault(self._pc_status, []). append(
                    [
                        coord_row,
                        tuple(row_values.values.flatten()),
                    ]
                )
                self.status_locations.setdefault(self._session_num, {}).update(status_map)

            elif re.match(r'[a-zA-Z]', row):
                continue
            elif re.findall(r'\d+(,\d+)?', row):
                row_values = pd.DataFrame([list(row.split(","))], columns=self._columns).dropna()
                self._df = self._df.append(row_values, ignore_index=True)
                self.df_map[self._session_num] = self._df
                coord_row += 1

    def load_json(self, json_filepath):
        with open(json_filepath, 'r') as jsonFile:
            return json.load(jsonFile)

    def split_into_phases(self, df_map):
        index_val = 0
        phase_map = {}
        for key, df in df_map.items():
            phase_map[key] = \
                {'ZERO' : df[: self.status_locations[key][PC_STATUS.PC_READY][1][index_val]],
                'PICK' : df[self.status_locations[key][PC_STATUS.PC_READY][1][index_val]:
                            self.status_locations[key][PC_STATUS.PC_AT_DROPOFF_XYZ][0][index_val]],
                'DROP' : df[self.status_locations[key][PC_STATUS.PC_AT_DROPOFF_XYZ][0][index_val]:]}

        return phase_map

    def _determine_status(self, current_status):
        status_mapping = {
            "PC_READY" : PC_STATUS.PC_READY,
            "PC_ZERO_RETURN" : PC_STATUS.PC_ZERO_RETURN,
            "PC_ZERO_RETURN_WAIT" : PC_STATUS.PC_ZERO_RETURN_WAIT,
            "PC_PICK_COMMAND_RECEIVED" : PC_STATUS.PC_PICK_COMMAND_RECEIVED,
            "PC_TARGET_FOUND" : PC_STATUS.PC_TARGET_FOUND,
            "PC_MOVING_ABOVE_PICK" : PC_STATUS.PC_MOVING_ABOVE_PICK,
            "PC_AT_PICK_POSITION_XY_ABOVE_Z" : PC_STATUS.PC_AT_PICK_POSITION_XY_ABOVE_Z,
            "PC_PROBING" : PC_STATUS.PC_PROBING,
            "PC_WAIT_FOR_MOTION" : PC_STATUS.PC_WAIT_FOR_MOTION,
            "PC_HAS_ITEM" : PC_STATUS.PC_HAS_ITEM,
            "PC_AT_PICK_POSITION_Z_CLEARANCE" : PC_STATUS.PC_AT_PICK_POSITION_Z_CLEARANCE,
            "PC_MOVING_TO_DROPOFF_XY" : PC_STATUS.PC_MOVING_TO_DROPOFF_XY,
            "PC_AT_DROPOFF_XY" : PC_STATUS.PC_AT_DROPOFF_XY,
            "PC_MOVING_TO_DROPOFF_XYZ" : PC_STATUS.PC_MOVING_TO_DROPOFF_XYZ,
            "PC_AT_DROPOFF_XYZ" : PC_STATUS.PC_AT_DROPOFF_XYZ,
            "PC_ITEM_PLACED" : PC_STATUS.PC_ITEM_PLACED
        }
        return status_mapping[current_status]