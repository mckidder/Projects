document.addEventListener('astilectron-ready', function() {

	astilectron.onMessage(function(message) {
		if (message.name === 'ip') {
			document.getElementById('ip').innerHTML = message.payload;
		} else if (message.name === 'receive') {
			document.getElementById('message-box').value += message.payload;
		}
	});

	document.getElementById('send').addEventListener('click', function() {
		let msg = document.getElementById('content-box').value;
		astilectron.sendMessage({name: 'send', msg});
	});

	document.getElementById('close').addEventListener('click', function() {
		astilectron.sendMessage({name: 'close'});
	});

	document.getElementById('connect').addEventListener('click', function() {
		let ip = document.getElementById('ip-box').value;
		astilectron.sendMessage({name: 'connect', payload: ip});
	});

	/*
	document.getElementById('file').addEventListener('click', function() {
		max = 0;
		astilectron.showOpenDialog({properties: ['openFile', 'singleSelection'],
			title: 'File to Encrypt/Decrypt'}, function(paths) {
				astilectron.sendMessage({name: 'open-file', payload: paths[0]});
		});
	});
	*/
});