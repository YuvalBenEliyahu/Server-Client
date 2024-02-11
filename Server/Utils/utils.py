DEFAULT_PORT_NUMBER = 1256


def get_port():
    port = DEFAULT_PORT_NUMBER
    try:
        with open('port.info', 'r') as file:
            port = file.readline().strip()
            port = int(port)
            return port
    except FileNotFoundError:
        print("File port.info not found. Using default port 1256.")
    except ValueError:
        print("Invalid port in port.info file. Using default port 1256.")
    finally:
        return port


