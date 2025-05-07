slim.load("console");
slim.load("csv");

// "/home/greergan/product/temp/GBG-export.csv"
const treasury_transactions_object = csv.read_document_sync("/home/greergan/Documents/GBG-export.csv");
const treasury_map_object = {};

console.dir(treasury_transactions_object.contents[0]);
//console.dir(treasury_transactions_object.contents);
/*
  player_id = {
	"name": "name",
	"date": {
		"transaction_type": {
			"era_name": {
				"good_name": value
			}
		}
	}
  }
*/

treasury_transactions_object.contents.forEach(entry => {
	if(!(entry["Player ID"] in treasury_map_object)) {
		treasury_map_object[entry["Player ID"]] = {
			"name": entry["Player name"]
		};
	}
	else {
		console.debug(entry["Player ID"] + " found");
	}
	/* date */
	const transaction_date = entry["Date/Time"].substring(0, entry["Date/Time"].indexOf(" "));
	if(!(transaction_date in treasury_map_object[entry["Player ID"]])) {
		console.debug(transaction_date + " not found");
		treasury_map_object[entry["Player ID"]][transaction_date] = {};
	}
	else {
		console.debug(transaction_date + " found");
	}
	/* transaction type */
	if(!(entry["Message"] in treasury_map_object[entry["Player ID"]][transaction_date])) {
		console.debug(entry["Message"] + " not found");
		treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]] = {};
	}
	else {
		console.debug(entry["Message"] + " found");
	}
	/* era */
	const era = entry["Era"].substring(entry["Era"].indexOf("-") + 2);
	if(!(era in treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]])) {
		console.debug(era + " not found");
		treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]][era] = {};
	}
	else {
		console.debug(era + " found");
	}
	/* good */
	if(!(entry["Good"] in treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]][era])) {
		console.debug(entry["Good"] + " not found");
		treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]][era][entry["Good"]] = 0;
	}
	else {
		console.debug(entry["Good"] + " found");
	}
	if(!("total" in treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]])) {
		console.debug("total" + " not found in transaction type");
		treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]].total = 0;
	}
	else {
		console.debug("total" + " found in transaction type");
	}
	if(!("total" in treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]][era])) {
		console.debug("total" + " not found in era");
		treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]][era].total = 0;
	}
	else {
		console.debug("total" + " found in era");
	}
	/* amount */
	treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]][era][entry["Good"]] += parseInt(entry["Amount"]);
	/* transaction type total */
	treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]].total += parseInt(entry["Amount"]);
	/* era total */
	treasury_map_object[entry["Player ID"]][transaction_date][entry["Message"]][era].total += parseInt(entry["Amount"]);
});

const report = {
	"blue":{
		"donations": {
			"sav": {},
			"sam": {}
		}
	},
	"others":{
		"donations": {
			"sav": {},
			"sam": {}
		}
	}
};

Object.keys(treasury_map_object).forEach(key1 => {
	if(treasury_map_object[key1].name === "Blue Wolf" || treasury_map_object[key1].name === "Ham-e" || treasury_map_object[key1].name === "Peter north") {
		console.debug(key1, treasury_map_object[key1].name);
		Object.keys(treasury_map_object[key1]).forEach(key2 => {
			if(key2 !== "name") {
				//if(treasury_map_object[key1][key2] === "2/17/2025") {
					console.info("\t", key2);
					Object.keys(treasury_map_object[key1][key2]).forEach(key3 => {
						if(key3 === "Guild treasury donation" || key3 === "Battlegrounds: Place Building" || key3 ==="Guild expedition: Difficulty unlocked") {
							console.info("\t\t", key3);
							Object.keys(treasury_map_object[key1][key2][key3]).forEach(key4 => {
								if(key4 === "total") {
									console.info("\t\t\ttotal", treasury_map_object[key1][key2][key3][key4]);
								} else if(key4 === "Space Age Mars" || key4 === "Space Age Venus") {
									console.info("\t\t\t", key4);
									Object.keys(treasury_map_object[key1][key2][key3][key4]).forEach(key5 => {
										console.info("\t\t\t\t", key5, treasury_map_object[key1][key2][key3][key4][key5]);
									});
								}
/* 								else {
									console.info("\t\t\t", key4);
									Object.keys(treasury_map_object[key1][key2][key3][key4]).forEach(key5 => {
										console.info("\t\t\t\t", key5, treasury_map_object[key1][key2][key3][key4][key5]);
									});
								} */
							});
						}
					});
				//}
			}
		});
	}
});
