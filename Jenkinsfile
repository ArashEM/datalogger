node('Build') {
    stage('Clone'){
       git 'git@gitlab.com:arash7/datalogger.git'
   }

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
