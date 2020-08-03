import React from "react";
import Styles from "./ChatBox.module.css";
export default function chatBox(props) {
	const { user, message, type } = props;
	const className =
		type === "sent"
			? Styles.sent
			: type === "received"
			? Styles.received
			: null;
	return (
		<div className={className}>
			<p>{message}</p>
		</div>
	);
}
