import React, { useState } from "react";
import JoinRoom from "./JoinRoom";
import CreateRoom from "./CreateRoom";
export default function Join() {
	const [showJoinRoom, setShowJoinRoom] = useState(false);
	const [showCreateRoom, setShowCreateRoom] = useState(true);
	const handleCreateRoom = () => {
		setShowCreateRoom(true);
		setShowJoinRoom(false);
	};
	const handleJoinRoom = () => {
		setShowCreateRoom(false);
		setShowJoinRoom(true);
	};

	return (
		<div className="container">
			<h1>Vchat</h1>
			<div className="wrapper">
				<button onClick={handleCreateRoom} className="btn">
					Create room
				</button>
				<button onClick={handleJoinRoom} className="btn">
					Join room
				</button>
			</div>
			<div className="options-container">
				{showCreateRoom && <CreateRoom></CreateRoom>}
				{showJoinRoom && <JoinRoom></JoinRoom>}
			</div>
		</div>
	);
}
