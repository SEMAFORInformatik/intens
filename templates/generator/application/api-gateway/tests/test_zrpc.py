import zrpc
import pytest
import platform

if platform.system() != 'Windows':
    @pytest.fixture()
    def client():
        c = zrpc.test_client()
        yield c
        c.stop_server()

    def test_autolog(client):
        r = client.autolog(dict(authHeader='YWRtaW46YWRtaW4='))
        assert r == [b'{"status": "ok"}',
                     b'{"authHeader": "YWRtaW46YWRtaW4=", "password": "YWRtaW4="}']

