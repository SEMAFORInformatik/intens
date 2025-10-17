{{- define "${application}.job.tpl" -}}
metadata:
  labels:
    {{- include "${application}.selectorLabels" . | nindent 4 }}
    app: ${application}-intens
spec:
  terminationGracePeriodSeconds: 0
  restartPolicy: Never
  {{- if .Values.affinity }}
  affinity:
    nodeAffinity:
      requiredDuringSchedulingIgnoredDuringExecution:
        nodeSelectorTerms:
        {{- range $key, $value := .Values.affinity }}
        - matchExpressions:
          - key: {{ $key }}
            operator: In
            values:
            - "{{ $value }}"
        {{- end }}
  {{- end }}
  containers:
    - image: "{{ .Values.app.image.repository }}:{{ .Values.app.image.tag | default .Chart.AppVersion }}"
      imagePullPolicy: {{ .Values.app.image.pullPolicy }}
      name: {{ .Chart.Name }}
      ports:
        - name: zmqrep
          containerPort: 15560
      env:
        - name: IS_WEBTENS
          value: "1"
        - name: REST_SERVICE_BASE
          value: "{{ .Values.app.restServiceBase }}"
        - name: API_GATEWAY_HOST
          value: {{ include "${application}.fullname" . }}-api-gateway
        {{- if .Values.otel.enabled }}
        - name: INTENS_OTEL_ENABLED
          value: "1"
        {{- end }}
      resources:
        requests:
          memory: "1500Mi"
        limits:
          memory: "4000Mi"

      volumeMounts:
      - mountPath: /home/intens
        name: cache-volume

      livenessProbe:
        tcpSocket:
          port: zmqrep

  volumes:
  - name: cache-volume
    emptyDir:
      sizeLimit: 20Mi

  imagePullSecrets:
    {{- if not .Values.existingImageCredentials }}
    - name: ${application}-registry
    {{- else }}
    - name: {{ .Values.existingImageCredentials }}
    {{- end }}
{{- end -}}
