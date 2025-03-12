import asyncio
import websockets
import json

CONNECTIONS = set()

def get_next_state(current_state):
    state_mapping = {
        "RED": "GREEN",
        "YELLOW": "RED",
        "GREEN": "YELLOW"
    }
    return state_mapping.get(current_state, current_state)

async def echo(websocket):
    if websocket not in CONNECTIONS:
        CONNECTIONS.add(websocket)
    async for message in websocket:
        print(f"Received message: {message}")
        try:
            data = json.loads(message)
            if "north" in data:
                data["north"] = get_next_state(data["north"])
            if "south" in data:
                data["south"] = get_next_state(data["south"])
            if "west" in data:
                data["west"] = get_next_state(data["west"])
            if "east" in data:
                data["east"] = get_next_state(data["east"])

            # Broadcast the updated traffic light states to all connected clients
            websockets.broadcast(CONNECTIONS, json.dumps(data))  # Ensure you send the updated JSON
        except json.JSONDecodeError:
            print("Invalid JSON received")

async def main():
    async with websockets.serve(echo, "0.0.0.0", 8765):
        print("WebSocket server started on ws://192.168.122.152:8765")
        await asyncio.Future()  # Run forever

asyncio.run(main())
