const express = require("express");
const socketio = require("socket.io");
const http = require("http");
const PORT = process.env.PORT || 5000;
const app = express();
const server = http.createServer(app);
const io = socketio(server);
const { v4: uuidv4 } = require("uuid");
const cors = require("cors");
let usersOnChat = [];
let storedRoomId;
app.use(cors());
app.all("/", (req, res) => {
	res.redirect(`/${uuidv4()}`);
});

app.get("/:roomId", (req, res) => {
	const roomId = req.params.roomId;
	res.json({ roomId: roomId });
});
io.on("connect", (socket) => {
	console.log("new user has joined the server");
	socket.on("join", ({ roomId, user }) => {
		storedRoomId = roomId;

		socket.join(roomId);
		socket.to(roomId).broadcast.emit("user-connected", { user });

		socket.to(storedRoomId).emit("users-on-chat", { user });
		console.log(roomId);
	});
	socket.on("send-message", ({ user, roomId, message }) => {
		console.log(user);
		console.log(roomId);
		console.log(message);
		socket.to(roomId).emit("message", { user: user, message: message });
	});

	/* socket.on("add-user", () => {
		s
	});
	 */
	socket.on("disconnect", (user) => {
		console.log("user has disconnected");
	});
});

server.listen(PORT, () => console.log(`Server running on port ${PORT}`));
