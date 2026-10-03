FROM --platform=linux/amd64 ubuntu:24.04

# The following 2 lines are added to avoid hanging the container creation. 
# See <https://grigorkh.medium.com/fix-tzdata-hangs-docker-image-build-cdb52cc3360d>
ENV TZ=Europe/Brussels
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install -y \
    locales sudo cppcheck clang-format

RUN groupadd -g 1100 dev \
        && useradd -u 1100 -g dev -d /home/dev dev \
        && mkdir /home/dev \
        && chown -R dev:dev /home/dev

# RUN sudo ln -s /usr/bin/clang-15 /usr/bin/clang

# The following 3 lines allow the 'dev' user to run sudo (password is "dev"). 
# Useful when later on packages need to be installed that are missing.
RUN usermod -aG sudo dev
SHELL ["/bin/bash", "-o", "pipefail", "-c"]
RUN echo 'dev:dev' | chpasswd

#
#   Now for some stuff required by the Mono build system.
#
# RUN ln -s /usr/bin/sed /usr/local/bin/gsed \
#         && ln -s /usr/bin/python3 /usr/bin/python
# RUN curl https://bootstrap.pypa.io/pip/2.7/get-pip.py --output get-pip.py \
#         && python get-pip.py \
#         && pip install jinja2 -v \
#         && rm get-pip.py \
#         && export PYTHONPATH=/usr/local/lib/python2.7/dist-packages:/usr/lib/python2.7/dist-packages

# RUN pip3 install jinja2

# RUN alias python='python3'

RUN locale-gen en_US.UTF-8

ENV LANG en_US.UTF-8

USER dev

WORKDIR /home/dev
