#!/usr/bin/env python
#
# Standard API Gateway Functions for Intens Applications
#
import logging

logger = logging.getLogger(__name__)


def autolog(request):
    """enable auto log in with authHeader
    extracts password from authHeader
    """
    try:
        import base64
        if request.get('authHeader'):
            # decode authHeader => encode password
            d = base64.b64decode(request['authHeader']).decode().split(':')
            if len(d) > 1:
                request['password'] = base64.b64encode(d[1].encode()).decode()
            return [{"status": "ok"}, request]

        if request.get('password'):
            # decode password
            request['password'] = base64.b64decode(request['password']).decode()
            return [{"status": "ok"}, request]
    except Exception:
        pass # be silent
    return [{"status": "ok"}, request]


def logbook(request):
    logbook = request
    if len(logbook) == 1:
        if isinstance(logbook[0], list):
            logbook = logbook[0]

    if logbook is None:
        return [{"status": "ok", "message": "No entries"},
                "",
                []]

    text = [
        "<style>",
        "body {",
        " background-color: #eeeeee;",
        "}",
        ".title {",
        " font-size: medium;",
        " background-color: #adc5e7;",
        " color: black;",
        " margin: 0px 5px 0px 5px;",
        " border: 10px 10px 10px 10px;",
        " white-space:pre;",
        "}",
        ".text {",
        " font-size: normal;",
        " color: black;",
        " margin: 0px 5px 0px 5px;",
        " white-space:pre;",
        "}",
        "</style>",
        "<body>"]

    position = []
    start_pos = 1

    if len(logbook) == 1:
        if not logbook[0].get('text', None):
            return [{"status": "ok"},
                    '\n'.join(text),
                    position]

    for n, entry in enumerate(logbook):
        created = entry.get('created', None)
        if created is None:
            created_date = "unknown"
        else:
            created_date = created
        title = ["#{}: {} ({})"
                 .format(n,
                         created_date,
                         entry.get('ownername', 'unknown'))]
        changed = entry.get('changed', None)
        if changed is not None:
            changed_date = changed
            if changed_date == created_date:
                changed = None

        if changed is not None:
            title.append(', modified: {} ({})'
                         .format(changed_date,
                                 entry.get('changername', 'unknown')))

        log_title = ''.join(title)
        text.append('<div class="title">')
        text.append('<p><b>{}</b></p>'.format(log_title))
        text.append('</div>')
        txt = entry.get('text', '<empty>')
        txt = txt.strip(' \n\t')

        lines = txt.split('\n')
        len_text = 0
        for line in lines:
            len_text += len(line) + 1
        text.append('<div class="text">')
        text.append('<p>{}</p>'.format('<br/>'.join(lines)))
        text.append('</div>')

        end_pos = start_pos + len(log_title) + len_text
        position.append({"start": start_pos,
                         "end": end_pos,
                         "index": n})
        start_pos = end_pos + 1
    text.append('<hr/>')
    text.append("</body>")

    return [{"status": "ok"},
            '\n'.join(text),
            position]
