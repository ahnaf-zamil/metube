import json
from flask import Response, abort, jsonify
from flask_inputs import Inputs
from flask_inputs.validators import JsonSchema
from future.utils import iteritems

from wtforms.form import BaseForm
from wtforms.fields import Field

import collections

class JsonInputs(Inputs):
    def __init__(self, schema, request):
        """Base class for input validation. Subclass to add validators.

        :param request: flask.Request object to validate.
        """

        self.json = [JsonSchema(schema=schema)]
        #: List of errors from all validators.
        self.errors = []

        self._request = request
        self._forms = dict()

        for name in dir(self):
            if not name.startswith('_') and name not in ['errors', 'validate', 'valid_attrs']:
                input = getattr(self, name)
                fields = dict()

                if isinstance(input, dict):
                    for field, validators in iteritems(input):
                        fields[field] = Field(validators=validators)
                elif isinstance(input, collections.abc.Iterable):
                    fields['_input'] = Field(validators=input)

                self._forms[name] = BaseForm(fields)


def validate_input(schema, request):
    inputs = JsonInputs(schema, request)
    if not inputs.validate():
        return abort(Response(json.dumps({"errors": inputs.errors}), 400, content_type="application/json"))
