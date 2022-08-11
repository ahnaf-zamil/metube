from __future__ import annotations
import json
from pika import BlockingConnection, URLParameters, exceptions

import os

conn_params = URLParameters(os.environ["AMQP_URI"])
conn_params.heartbeat = 300
conn_params.blocked_connection_timeout = 300

class MQPublisher:
    def __init__(self, queue, exchange = ''):
        self.exchange = exchange
        self.queue = queue
        self._conn = None
        self._channel = None

    def connect(self) -> MQPublisher:
        if not self._conn or self._conn.is_closed:
            self._conn = BlockingConnection(conn_params)
            self._channel = self._conn.channel()
            self._channel.queue_declare(self.queue, durable=True)
        return self

    def _publish(self, msg: dict):
        self._channel.basic_publish(self.exchange, self.queue, json.dumps(msg))

    def publish(self, msg: dict):
        try:
            self._publish(msg)
        except exceptions.AMQPError:
            self.connect()._publish(msg)
