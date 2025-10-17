pipeline {
  agent none

  environment {
    SONAR_TOKEN = "sqp_d17bb64f6eb6150cdd23560d8937c16f6dc25a67"
  }

  stages {
    stage("Build") {
      agent {
        dockerfile {
          filename "docker/Dockerfile-linux-build"
          args "-u root"
          registryUrl "https://hub.semafor.ch"
          registryCredentialsId "hub-push"
        }
      }
      steps {
        gitlabCommitStatus("build") {
          sh("nice cmake -B build . -DCMAKE_EXPORT_COMPILE_COMMANDS=on -DUSE_LSP=ON -DUSE_QT6=ON")
          sh("nice cmake --build build -j9")
        }
      }
      // post {
      //   always {
      //     archiveArtifacts artifacts: "build/**", fingerprint: true
      //   }
      // }
    }
    stage("Test") {
      agent {
        dockerfile {
          filename "docker/Dockerfile-linux-build"
          args "-u root"
          registryUrl "https://hub.semafor.ch"
          registryCredentialsId "hub-push"
        }
      }
      steps {
        gitlabCommitStatus("test") {
          sh("cd build && PATH=${env.WORKSPACE}/build/src:/opt/venv/bin:${env.PATH} QT_QPA_PLATFORM=offscreen ctest -V --output-junit testRes.xml --timeout 300")
        }
      }
      post {
        always {
          junit "build/testRes.xml"
        }
      }
    }
    stage("Docker") {
      agent {
        docker {
          image "docker:latest"
          args '-u root -v /var/run/docker.sock:/var/run/docker.sock'
        }
      }
      when {
        buildingTag()
      }
      steps {
        script {
          gitlabCommitStatus("docker") {
            docker.withRegistry('https://hub.semafor.ch', 'hub-push') {
              sh("docker buildx bake default dev --push")
            }
          }
        }
      }
    }

    stage("Sonar") {
      when {
        branch "oss"
      }
      agent {
        docker {
          image "hub.semafor.ch/semafor/intens/linux-build:latest"
        }
      }
      steps {
        sh '''
          sonar-scanner \
            -Dsonar.projectKey=semafor_intens_360a5dac-82d3-47d8-ad7f-16ea43a148de \
            -Dsonar.cfamily.compile-commands=build/compile_commands.json \
            -Dsonar.sources=. \
            -Dsonar.host.url=http://sonar.semafor.ch
        '''
      }
    }
  }
}
