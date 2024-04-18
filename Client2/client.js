const express = require('express');
const app = express();
const net = require('net');
const bodyParser = require('body-parser');
const path = require('path');

const PORT = 5060;
const PORT2 = 5002;
const HOST = '127.0.0.1';
let turno = 0;
var meToca = false;
var flag = false;
let tablero = Array.from({ length: 6 }, () => Array.from({ length: 7 }, () => 0));

app.use(bodyParser.urlencoded({ extended: true }));
app.use(express.json());
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');
app.use('/socket.io', express.static(path.join(__dirname, 'node_modules/socket.io/client-dist')));

function preguntarTurno() {
    if(!meToca){
        const client = new net.Socket();

        client.connect(PORT, HOST, () => {
            //console.log(`${turno}:Me toca?`);
            client.write(`${turno}:Me toca?`);
        });

        client.on('data', (data) => {
            const respuesta = data.toString();
            console.log(`Respuesta del servidor: ${respuesta}`);

            // Dividir la respuesta en dos partes usando el delimitador ":"
            const partes = respuesta.split(':');

            // partes[0] contendrá "SI" y partes[1] contendrá "Tu turno"
            const primeraParte = partes[0];
            const segundaParte = partes[1];

            meToca = primeraParte === "Si";
            if(primeraParte === "Si"){
                tablero = JSON.parse(segundaParte);
                console.log("drtfgyvhbujnkml: ");
                for (let fila of tablero) {
                    console.log(fila.join(' '));
                }
            }
            
            client.destroy();
        });

        client.on('close', () => {
            console.log('Conexión cerrada');
        });

        client.on('error', (err) => {
            console.error(`Error: ${err}`);
        });
    }
}

setInterval(preguntarTurno, 4000);

app.get('/', (req, res) => {
    res.render('index', { message: '' });
});

app.post('/send', (req, res) => {
    const { col } = req.body;
    console.log('Mensaje recibido del cliente: ', col);

    // Encuentra la primera fila disponible en la columna especificada
    let fila = 5;
    while (fila >= 0 && tablero[fila][col] !== 0) {
        fila--;
        
    }
    console.log('Fila: ', fila);
    if (fila >= 0) {
        tablero[fila][col] = turno;
        console.log('Tablero actualizado:', tablero);
    } else {
        console.log('Columna llena, no se puede colocar la ficha');
    }

    const client = new net.Socket();
    client.connect(PORT, HOST, () => {
        //console.log('Conexion establecida con el servidor');
        let mensaje = turno.toString() + ":" + JSON.stringify(tablero);
        console.log(`Mensaje enviado: ${mensaje}`);
        client.write(mensaje);
    });
    client.on('data', (data) => {
        console.log(`Respuesta del servidor: ${data}`);
        client.destroy();
    });

    res.json({ message: 'Columna recibida correctamente' });
    meToca = false;
});

app.post('/login', (req, res) => {
    const username = req.body.username;
    const password = req.body.password;

    const client = new net.Socket();

    client.connect(PORT, HOST, () => {
        //console.log('Conexion establecida con el servidor');

        const credentials = `${username}:${password}`;
        client.write(credentials);
    });

    client.on('data', (data) => {
        console.log(`Respuesta del servidor: ${data}`);
        if(!flag){
            turno = data.toString();
            flag = true;
        }

        if (turno != "0") {
            res.redirect('/juego');
        } else {
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
    res.render('juego', { meToca: meToca, tablero: tablero }); // Renderiza la vista del juego de Conecta 4
});

// Ruta para manejar cualquier otra solicitud no encontrada
app.use((req, res) => {
    res.status(404).send('P�gina no encontrada');
});

app.listen(PORT2, () => {
    console.log(`Servidor Express iniciado en http://localhost:${PORT2}`);
});