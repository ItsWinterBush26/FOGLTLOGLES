FROM ubuntu

SHELL ["/bin/bash", "-c"]

RUN sudo apt-get update \
    && sudo apt-get install -y ninja-build wget \
    && sudo apt-get clean
            
RUN sudo rm -rf /var/cache/apt/* \
    && sudo rm -rf /var/lib/apt/lists/* \
    && sudo rm -rf /tmp/*

# cmake

ENV HOME_PATH="/home/gitpod"
ENV SDKMAN_PATH="${HOME_PATH}/.sdkman"

# java
RUN rm -rf ${SDKMAN_PATH} && curl -s 'https://get.sdkman.io' | bash
RUN source "${SDKMAN_PATH}/bin/sdkman-init.sh" && sdk install java 21.0.5-graal

ENV ANDROID_SDK_ROOT="${HOME_PATH}/android-sdk"
ENV ANDROID_HOME="${ANDROID_SDK_ROOT}"

# sdkmanager
RUN cd ${HOME_PATH} && wget https://dl.google.com/android/repository/commandlinetools-linux-11076708_latest.zip
RUN cd ${HOME_PATH} && unzip commandlinetools*.zip && rm commandlinetools*.zip

