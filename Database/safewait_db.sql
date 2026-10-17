CREATE DATABASE IF NOT EXISTS safewait_db 
DEFAULT CHARACTER SET utf8mb4 
COLLATE utf8mb4_unicode_ci;

USE safewait_db;

-- catalogo de rutas de transporte
CREATE TABLE rutas (
    ruta_id INT AUTO_INCREMENT NOT NULL,
    nombre_ruta VARCHAR(50) NOT NULL,
    color VARCHAR(30) NULL,
    valor_r SMALLINT NULL,
    valor_g SMALLINT NULL,
    valor_b SMALLINT NULL,
    CONSTRAINT rutas_pk PRIMARY KEY (ruta_id)
) ENGINE=InnoDB;

-- informacion de los paraderos
CREATE TABLE paraderos (
    paradero_id INT AUTO_INCREMENT NOT NULL,
    nombre VARCHAR(100) NOT NULL,
    ubicacion VARCHAR(150) NULL,
    activo BOOLEAN DEFAULT TRUE,
    CONSTRAINT paraderos_pk PRIMARY KEY (paradero_id)
) ENGINE=InnoDB;

-- relacion para ver que rutas pasan por cual paradero
CREATE TABLE paradero_rutas (
    paradero_id INT NOT NULL,
    ruta_id INT NOT NULL,
    CONSTRAINT paradero_rutas_pk PRIMARY KEY (paradero_id, ruta_id),
    CONSTRAINT fk_paradero_rutas_paradero FOREIGN KEY (paradero_id) REFERENCES paraderos(paradero_id) ON DELETE CASCADE ON UPDATE CASCADE,
    CONSTRAINT fk_paradero_rutas_ruta FOREIGN KEY (ruta_id) REFERENCES rutas(ruta_id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB;

-- lista de usuarios registrados
CREATE TABLE usuarios (
    usuario_id INT AUTO_INCREMENT NOT NULL,
    nombre VARCHAR(50) NOT NULL,
    apellido VARCHAR(50) NOT NULL,
    categoria_vulnerabilidad VARCHAR(20) NULL,
    CONSTRAINT usuarios_pk PRIMARY KEY (usuario_id)
) ENGINE=InnoDB;

-- tarjetas nfc dadas de alta
CREATE TABLE tarjetas_nfc (
    tarjeta_id INT AUTO_INCREMENT NOT NULL,
    uid_nfc VARCHAR(30) NOT NULL UNIQUE,
    usuario_id INT NOT NULL,
    autorizada BOOLEAN DEFAULT TRUE,
    CONSTRAINT tarjetas_nfc_pk PRIMARY KEY (tarjeta_id),
    CONSTRAINT fk_tarjetas_usuario FOREIGN KEY (usuario_id) REFERENCES usuarios(usuario_id) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB;

-- modulos esp8266 instalados
CREATE TABLE nodos_iot (
    nodo_id INT AUTO_INCREMENT NOT NULL,
    paradero_id INT NOT NULL,
    nombre_nodo VARCHAR(60) NOT NULL,
    tipo_nodo VARCHAR(20) NOT NULL,
    sensor VARCHAR(50) NULL,
    actuador VARCHAR(50) NULL,
    CONSTRAINT nodos_iot_pk PRIMARY KEY (nodo_id),
    CONSTRAINT fk_nodos_paradero FOREIGN KEY (paradero_id) REFERENCES paraderos(paradero_id) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB;

-- historial de accesos con tarjeta
CREATE TABLE registros_acceso (
    acceso_id INT AUTO_INCREMENT NOT NULL,
    tarjeta_id INT NOT NULL,
    nodo_id INT NOT NULL,
    estado_acceso VARCHAR(12) NOT NULL,
    fecha_hora DATETIME NOT NULL,
    CONSTRAINT registros_acceso_pk PRIMARY KEY (acceso_id),
    CONSTRAINT fk_acceso_tarjeta FOREIGN KEY (tarjeta_id) REFERENCES tarjetas_nfc(tarjeta_id) ON DELETE RESTRICT ON UPDATE CASCADE,
    CONSTRAINT fk_acceso_nodo FOREIGN KEY (nodo_id) REFERENCES nodos_iot(nodo_id) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB;

-- detecciones del sensor de color de las rutas
CREATE TABLE detecciones_ruta (
    deteccion_id INT AUTO_INCREMENT NOT NULL,
    nodo_id INT NOT NULL,
    ruta_id INT NOT NULL,
    valor_r SMALLINT NULL,
    valor_g SMALLINT NULL,
    valor_b SMALLINT NULL,
    fecha_hora DATETIME NOT NULL,
    CONSTRAINT detecciones_ruta_pk PRIMARY KEY (deteccion_id),
    CONSTRAINT fk_deteccion_nodo FOREIGN KEY (nodo_id) REFERENCES nodos_iot(nodo_id) ON DELETE RESTRICT ON UPDATE CASCADE,
    CONSTRAINT fk_deteccion_ruta FOREIGN KEY (ruta_id) REFERENCES rutas(ruta_id) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB;

-- logs de niveles de luz
CREATE TABLE registros_iluminacion (
    registro_id INT AUTO_INCREMENT NOT NULL,
    nodo_id INT NOT NULL,
    nivel_luz SMALLINT NOT NULL CHECK (nivel_luz >= 0 AND nivel_luz <= 100),
    estado_luces VARCHAR(10) NOT NULL,
    fecha_hora DATETIME NOT NULL,
    CONSTRAINT registros_iluminacion_pk PRIMARY KEY (registro_id),
    CONSTRAINT fk_iluminacion_nodo FOREIGN KEY (nodo_id) REFERENCES nodos_iot(nodo_id) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB;

-- historial de panico sos
CREATE TABLE alertas_sos (
    alerta_id INT AUTO_INCREMENT NOT NULL,
    nodo_id INT NOT NULL,
    duracion_segundos SMALLINT NOT NULL CHECK (duracion_segundos >= 0),
    fecha_hora DATETIME NOT NULL,
    CONSTRAINT alertas_sos_pk PRIMARY KEY (alerta_id),
    CONSTRAINT fk_alertas_nodo FOREIGN KEY (nodo_id) REFERENCES nodos_iot(nodo_id) ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB;