const express = require('express');
const app = express();
const net = require('net');
const bodyParser = require('body-parser');
const path = require('path');

const PORT = 3000;
const HOST = '127.0.0.1';
let turno = 0; // Utiliza let en lugar de var para reasignar el valor de turno

app.use(bodyParser.urlencoded({ extended: true }));
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

// P�gina inicial con formulario de inicio de sesi�n
app.get('/', (req, res) => {
    res.render('index', { message: '' }); // Renderiza la vista index con un mensaje vac�o
});

// Ruta para procesar el formulario de inicio de sesi�n
app.post('/login', (req, res) => {
    const username = req.body.username;
    const password = req.body.password;

    const client = new net.Socket();

    client.connect(PORT, HOST, () => {
        console.log('Conexi�n establecida con el servidor');

        const credentials = `${username}:${password}`;
        client.write(credentials);
    });

    client.on('data', (data) => {
        console.log(`Respuesta del servidor: ${data}`);
        const response = data.toString();
        const partes = response.split(':');
        turno = parseInt(partes[0]); // Convierte turno a n�mero entero

        if (turno !== 0) {
            // Redirige a la p�gina de juego de Conecta 4 si turno es diferente de 0
            res.redirect('/juego');
        } else {
            // Renderiza la vista index con un mensaje de usuario no encontrado si turno es 0
            res.render('index', { message: 'Usuario no encontrado' });
        }

        client.destroy();
    });

    client.on('close', () => {
        console.log('Conexi�n cerrada');
    });

    client.on('error', (err) => {
        console.error(`Error: ${err}`);
        res.send(`<h2>Error al conectar con el servidor</h2>`);
    });
});

// Ruta para la p�gina del juego de Conecta 4
app.get('/juego', (req, res) => {
    res.render('juego'); // Renderiza la vista del juego de Conecta 4
});

// Ruta para manejar cualquier otra solicitud no encontrada
app.use((req, res) => {
    res.status(404).send('P�gina no encontrada');
});

app.listen(PORT, () => {
    console.log(`Servidor Express iniciado en http://localhost:${PORT}`);
});
