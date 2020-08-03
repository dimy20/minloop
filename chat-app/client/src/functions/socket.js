import io from "socket.io-client";
const ENDPOINT = "http://localhost:5000";
let socket;

export const initiateSocket = ({ roomId, user }, cb) => {
	socket = io(ENDPOINT);
	if (socket && roomId) {
		socket.emit("join", { roomId, user });
		return cb(null, user);
	}
};
export const notifyUsers = () => {
	socket.on("user-connected", ({ user }) => {
		console.log(`user ${user} has connected`);
	});
};
export const addUser = () => {
	socket.emit("add-user");
};
export const usersOnChat = (cb) => {
	socket.on("users-on-chat", ({ user }) => {
		return cb(null, user);
	});
};
export const disconnectSocket = () => {
	if (socket) {
		socket.emit("disconnect");
		socket.off();
	}
};
export const suscribteToChat = (cb) => {
	if (!socket) return true;
	socket.on("message", ({ user, message }) => {
		return cb(null, { user, message });
	});
};
export const SendMessage = ({ user, roomId, message }) => {
	if (socket) socket.emit("send-message", { user, roomId, message });
};
