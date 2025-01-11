import json


def make_grid(x, y, w, h, nx, ny):
    for j in range(ny):
        for i in range(nx):
            yield (x + i * w / nx, y + j * h / ny)


data = {
    "nodes": [],
    "teams": [],
    "base_food": 50
}

xg = 0
yg = 0
teams_id = 1
for i, (x, y) in enumerate(make_grid(0, 0, 60, 60, 6, 6)):
    data["nodes"].append({"id": i, "x": x, "y": y, "neighbors": [], "type": "NORMAL"})
    if xg == 2 and yg == 2 or xg == 3 and yg == 2 or xg == 2 and yg == 3 or xg == 3 and yg == 3:
        data["nodes"][-1]["type"] = "REINE"
        data["nodes"][-1]["team"] = teams_id
        teams_id += 1
    elif xg == 0 and yg == 0 or xg == 5 and yg == 0 or xg == 0 and yg == 5 or xg == 5 and yg == 5:
        data["nodes"][-1]["type"] = "NOURRITURE"
        data["nodes"][-1]["initial_food"] = 5
        data["nodes"][-1]["max_food"] = 10
        data["nodes"][-1]["regen"] = 2
        data["nodes"][-1]["total_available"] = 500
    elif xg % 2 == 0 and yg % 2 == 0:
        data["nodes"][-1]["type"] = "EAU"

    if xg < 5:
        data["nodes"][-1]["neighbors"].append({"to": i + 1, "life": 1})
    if yg < 5:
        data["nodes"][-1]["neighbors"].append({"to": i + 6, "life": 1})

    xg += 1
    if xg == 6:
        xg = 0
        yg += 1

print(teams_id)
data["teams"] = [
    {"id": i, "name": f"Team {i}", "color": f"#{i*10:02x}{i*10:02x}{i*10:02x}"}
    for i in range(1, teams_id )
]

with open("grid.json", "w") as f:
    json.dump(data, f)
