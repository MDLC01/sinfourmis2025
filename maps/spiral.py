import json
import math


def make_circle(x, y, r, nb, offset=0):
    offset = offset * math.pi / 180  # convert degrees to radians
    for i in range(nb):
        angle = 2 * math.pi * i / nb + offset
        yield (x + r * math.cos(angle), y + r * math.sin(angle))

data = {}
data["nodes"] = []
i = 0
# make a spiral of nodes
nb_nodes = 122 // 6 + 1
teams_spacing = nb_nodes // 4
water_spacing = nb_nodes // 6
teams_id = 0
queens_nodes = []
for anneau in range(1, 6):
    radius = anneau * 12.8
    offset = anneau * (39 - anneau * 2)
    spacing = teams_spacing
    wspacing = 3
    for x, y in make_circle(0, 0, radius, nb_nodes, offset):
        data["nodes"].append({"id": i, "x": x, "y": y, "type": "NORMAL"})
        spacing -= 1
        if spacing != 0:
            wspacing -= 1
            
        if spacing == 0:
            spacing = teams_spacing
            if anneau == 5:
                teams_id += 1
                data["nodes"][-1]["type"] = "REINE"
                data["nodes"][-1]["team"] = teams_id
                queens_nodes.append(i)
            elif anneau == 4:
                data["nodes"][-1]["type"] = "EAU"
            elif anneau == 2:
                data["nodes"][-1]["type"] = "NOURRITURE"
                data["nodes"][-1]["initial_food"] = 0
                data["nodes"][-1]["max_food"] = 100
                data["nodes"][-1]["regen"] = 20
                data["nodes"][-1]["total_available"] = 100
        elif anneau == 3 and wspacing == 0:
            wspacing = water_spacing
            spacing = teams_spacing
            data["nodes"][-1]["type"] = "EAU"

        data["nodes"][-1]["neighbors"] = []
        if anneau < 5:
            data["nodes"][-1]["neighbors"].append({"to": i + nb_nodes, "life": 0.5})
        if (i + 1) % nb_nodes != 0:
            data["nodes"][-1]["neighbors"].append({"to": i + 1, "life": 0.5})
        else:
            data["nodes"][-1]["neighbors"].append({"to": i - nb_nodes + 1, "life": 0.5})

        first = False
        i += 1

for id in queens_nodes:
    for j in [id - 1, id + 1]:
        j = j % nb_nodes + (nb_nodes * 4)
        data["nodes"][j]["type"] = "NOURRITURE"
        data["nodes"][j]["initial_food"] = 0
        data["nodes"][j]["max_food"] = 100
        data["nodes"][j]["regen"] = 20
        data["nodes"][j]["total_available"] = 100

data["nodes"].append({
    "id": i, 
    "x": 0, 
    "y": 0,
    "neighbors": [{"to": i, "life": 1} for i in range(nb_nodes)],
    "type": "NOURRITURE",
    "initial_food": 0,
    "max_food": 100,
    "regen": 20,
    "total_available": -1
})

data["base_food"] = 50
data["teams"] = [
    {"id": i, "name": f"Team {i}", "color": f"#{i*10:02x}{i*10:02x}{i*10:02x}"} for i in range(1, teams_id + 1)
]
print(i)
print(teams_id)

with open("spiral.json", "w") as f:
    json.dump(data, f)
