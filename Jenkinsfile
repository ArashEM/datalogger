node('Build') {
    stage('Prepare'){
        sh 'make clean'
        sh 'git pull'
    }
    
    stage('Build') {
        sh 'make'
    }
    
    stage('Archive'){
        archiveArtifacts 'datalogger'
    }
}
