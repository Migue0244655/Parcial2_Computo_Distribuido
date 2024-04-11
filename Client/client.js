const express = require('express');
const app = express();
const net = require('net');
const bodyParser = require('body-parser');
const path = require('path');

const PORT = 3000;
const HOST = '127.0.0.1';

app.use(bodyParser.urlencoded({ extended: true }));
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

app.get('/', (req, res) => {
    res.render('index');
});

app.post('/login', (req, res) => {
    const username = req.body.username;
    const password = req.body.password;

    const client = new net.Socket();

    client.connect(PORT, HOST, () => {
        console.log('Conexion establecida con el servidor');

        const credentials = `${username}:${password}`;
        client.write(credentials);
    });

    client.on('data', (data) => {
        console.log(`Respuesta del servidor: ${data}`);
        const response = data.toString();
        res.send(`<h2>${response}</h2>`);
        client.destroy();
    });

    client.on('close', () => {
        console.log('Conexión cerrada');
    });

    client.on('error', (err) => {
        console.error(`Error: ${err}`);
        res.send(`<h2>Error al conectar con el servidor</h2>`);
    });
});

app.listen(PORT, () => {
    console.log(`Servidor Express iniciado en http://localhost:${PORT}`);
});