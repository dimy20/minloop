import React, { useEffect, useState, useRef } from "react";
import Styles from "./Chat.module.css";
import ChatBox from "../components/ChatBox/ChatBox";
import queryString from "query-string";
import {
	initiateSocket,
	suscribteToChat,
	SendMessage,
	disconnectSocket,
	notifyUsers,
	usersOnChat,
	addUser,
} from "../functions/socket";
import Users from "../components/Users/Users";
export default function Socket(props) {
	const { roomId, user } = queryString.parse(props.location.search);
	const [message, setMessage] = useState("");
	const [chat, setChat] = useState([]);
	const [users, setUsers] = useState([]);
	useEffect(() => {
		if (roomId)
			initiateSocket({ roomId, user }, (err, user) => {
				if (err) return;
			});
		notifyUsers();
		addUser();
		usersOnChat((err, user) => {
			if (err) return;

			setUsers((prev) => {
				if (!prev.includes(user)) {
					return [...prev, user];
				}
			});
		});

		suscribteToChat((err, { user, message }) => {
			if (!err) {
				setChat((prev) => [
					...prev,
					{
						user: user,
						message: message,
						type: "received",
					},
				]);

				console.log(`${user} sent ${message}...`);
			} else return;
		});

		return () => {
			disconnectSocket();
		};
	}, [roomId]);
	const handleEnter = (e) => {
		e.target.value = "";
		setChat([
			...chat,
			{
				user: user,
				message: message,
				type: "sent",
			},
		]);
		SendMessage({ user, roomId, message });
	};
	useEffect(() => {
		setUsers((prev) => [...prev, "you"]);
	}, []);

	return (
		<React.Fragment>
			{/* {console.log(users)} */}
			<h1 style={{ marginLeft: "6em" }}>Chat</h1>
			<div className={Styles.container}>
				<Users users={users}></Users>
				<div className={Styles.Chatcontainer}>
					<div className={Styles.ChatWrapper}>
						<div className={Styles.chat}>
							{chat.map((m) => {
								return (
									<ChatBox
										key={m.message}
										user={m.user}
										message={m.message}
										type={m.type}
									></ChatBox>
								);
							})}
						</div>
						<input
							onChange={(e) => setMessage(e.target.value)}
							onKeyPress={(e) => (e.key === "Enter" ? handleEnter(e) : null)}
							className={Styles.input}
							placeholder="say hi!"
						></input>
					</div>
				</div>
			</div>
		</React.Fragment>
	);
}
