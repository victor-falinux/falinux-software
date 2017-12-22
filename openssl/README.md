# OpenSSL

TLS와 SSL의 오픈 소스 구현판이다. 기본적인 암호화 및 유틸리티를 제공한다.

## TLS, SSL

전송 계층 보안 (Transport Layer Security), 보안 소켓 레이어 (Secure Sockets Layer)

TLS는 SSL을 바탕으로 한다. SSL의 버전이 갱신되면서 TLS로 명칭이 바뀌었다. 앞으로 TLS와 SSL을 통칭해 TLS라 부른다.

TCP/IP 네트워크 통신에서 전송 계층 종단 간 보안과 데이터 무결성을 확보한다.

### 보안 방법

1. TLS 클라이언트(이하 클라이언트)가 자신의 암호화 정보를 나열하는 "클라이언트 안녕" 메시지를 TLS 서버(이하 서버)에 보낸다.
2. 서버가 클라이언트의 암호화 정보 중 자신이 지원하는 목록을 나열하고 자신의 인증서를 포함해 "서버 안녕" 메시지를 보낸다.
3. 클라이언트가 서버의 인증서를 확인한다.
4. 클라이언트가 앞으로 암호화에 사용할 비밀 키를 서버의 공개 키로 암호화해 송신한다.
5. 클라이언트가 "완료" 메시지를 비밀 키로 암호화해 서버에 보낸다.
6. 서버가 "완료" 메시지를 비밀 키로 암호화해 클라이언트에 보낸다.
7. 이제 비밀 키를 외부에 노출하지 않고 서로 나누게 되었다.

### 인증서

공개 키가 지정된 엔티티에 속하는지 확인한다. 누군가 공개 키를 위장해 메시지를 가로채는 걸 방지한다. 인증서는 인증 기관에서 발행한다. 인증 기관은 인증서의 공개 키가 인증서 신청자의 공개 키인 걸 보장한다.

### 용어

#### CA

인증 기관 (Certificate Authority)

#### CSR

인증서 서명 요청 (Certificate Signing Request)

공개 키와 기타 정보로 구성한다. 이 정보는 인증서에 서명할 때 같이 삽입한다.

#### DN

식별 정보 (Distinguished Name)

CSR을 구성하는 정보

#### CN

공용 이름 (Common Name)

DN 중 가장 중요한 요소. 보통 인증서를 사용할 전체 도메인 이름(FQDN, Fully Qualified Domain Name)으로 넣는다.

**주의: 다르게 적으면 호스트 검증에 실패한다.**

#### SSC

직접 서명한 인증서 (Self Signed Certificate)

본인 자신이 CA가 된다.

### 절차

#### 개인 키 및 인증서 생성

시험용으로 CA가 서명한 인증서 대신 SSC를 사용한다. CN은 localhost로 한다.

```
$ openssl req \
       -newkey rsa:2048 -sha256 -nodes -keyout localhost.key \
       -x509 -days 365 -out localhost.crt
$ ls
localhost.crt  localhost.key
```

#### 구현

간단하게 에코 서버와 클라이언트를 만들었다.

언어는 Go, github.com에 공개된 spacemonkeygo의 openssl 패키지를 사용했다.

## RSA

### 키 쌍 생성

두 가지 방법이 있다. ssh-keygen을 이용하는 방법과 openssl을 이용하는 방법이다.

#### ssh-keygen

```
$ ssh-keygen -N "" -f key
Generating public/private rsa key pair.
Your identification has been saved in key.
Your public key has been saved in key.pub.
The key fingerprint is:
SHA256:QKvndVsqCEkgzNeWyttnAtvVWc+aHaQi9JcjNhLshrE victor@gram
The key's randomart image is:
+---[RSA 2048]----+
|+ . . +          |
| + o * =   . .   |
|  o + O + o *    |
|   = E B O = +   |
|    X + S * * .  |
|   o B = . * .   |
|      * . o      |
|         .       |
|                 |
+----[SHA256]-----+
$ ssh-keygen -f key.pub -e -m pem > key.pem
$ ls
key  key.pem  key.pub
```

`key`와 `key.pem`은 OpenSSL에서 사용할 수 있는 개인/공개 키다. `key.pub`은 OpenSSH 전용 공개 키다. 내용은 다음과 같다.

```
$ cat key
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA99Ix04pR4c8/Zf97WKnNse7DRU/tUu+4YwPEy8r0LIAqB2IB
X4I2/g4kG1k9w6B3g1Xc5YI9LIFs0Sk1YdJg833gezym8ebdEkz6KRP5gUlgqx27
tjLiqs+aoX/PPxik99zyHxj6SFRvdVVV3IcRL9Pzn7vQ6O6/yqcKayXUSpQJ2n6B
GRV94e9R/oWh5r6dl5ZEck6uQcOIwWRf+xbLrjF/BwdsGxSc+HhnNoGv2zPQn5CD
Y8LgsTAUKNrs1+owqrINKQVoR5A2o6CmBdXCwZbS1l6RlWAMPo2N6c1FVqPGDVYu
1TdkesOiKT6SkeF2hHFUTWgGg+3LnDi81soYZQIDAQABAoIBAQDJyknwKk0KRasl
7qYNSBbJr9Uo9zATvEazuTnhh1zTb3jJki/0CR+mNvWHn17/kAZA1wWLjEUrnCdz
GM7y3E89dV34eyalCoswf6WjFTSw5YLlUsyqSzzuQSV0I5IpmnOZsDsmvzvs8XPg
YqALdZ+1Biz2AATX5rAc329IWAFIxzAXL/HC4WRTDpxyPpLCWISI1S3gdbUs8erg
q+oW43ipjzXoYwJsUYFvkkOHtz47M5IOFn8ZDGqEEsDJxwnTexGygqJ1Jra3VJQF
5LY+T81BQfsSmwR8zremAof1qJB1IkoEqiDb04PlVz4qSrMW8C3lvqUPw6lCJ/7c
5uQ8JFWBAoGBAP6rPyqdXTU3Dl7dFIyluHW3lMvhzuzOjIA9qdfB/XOb/vxuk63w
eUdyJmdbF7evfy+G2+2YaPkKSd00yAsWec6IOG+QsXsmr5s/Tje5UpmT6gqAABns
cNbNxwx035EjQFyS09wxrqfR2K9MIdDu8Lh6EX/jp0jRflOciH2ZewVJAoGBAPkd
yQiGLpGGnd4Tg+cPzs0MExe1ukYvxSXI1Xq4Qb0sPMJ1VPgtiwPZ7ZaACr6SqjkL
xPeBubetXCBxNLHZDmsC9xZt0ZOGfIBS1lq2LRm0/8reFSGsy5KZdvoniXNaYuXi
uUDmGU7WV5lW/xpR7ZdifMqnTsDuZRoQgj7hKSY9AoGAUj2jDHP9Ia5B6cijVjg+
NWWvm5e0Zyzcmp1PN+nhm3AJDCP5dPZvc+8dD8uPATafcA2xAC1z2NjFubXN1CbR
9yxubTjg19BKbWwoKj6iug3WSsXDQpvkZLnXSeKEQC6OHlccK8Ovj2po+1MuqHZB
LpHdDNvLrexZS08FlOn3nGECgYEA+K7aM+83Z8D5sWrL3wrxx9HI8X9Rj6U6aoi9
S7/KqF0T3o+gngtg3hxcSERT9YCV+1KbcvUGf6zI7f30u6HR+dPA85v3fgzgFn4b
S/zWsvXz8AGZGL/VjOg0VG7JkOTnG1Qk2Puawg/I7ONi0FR/GQORTgCM5p5VQRq/
2qONH4UCgYEAkvZvL+RG9iv20YnEi4d1kEwq6DsB90EuxiWO37KVZwJ7lIug6tC0
3FPVlRoMcNYekQcvuUsBaHE2WjmGrSwKOeG+4W9oS7coPulthAlhZeyJJ2/qm1Wr
6a9IiwMyy9V0MPRYyrs/cYSUWQeYKNKi8HfaRCLtkfofGFyvSSAsRJs=
-----END RSA PRIVATE KEY-----
$ cat key.pem
-----BEGIN RSA PUBLIC KEY-----
MIIBCgKCAQEA99Ix04pR4c8/Zf97WKnNse7DRU/tUu+4YwPEy8r0LIAqB2IBX4I2
/g4kG1k9w6B3g1Xc5YI9LIFs0Sk1YdJg833gezym8ebdEkz6KRP5gUlgqx27tjLi
qs+aoX/PPxik99zyHxj6SFRvdVVV3IcRL9Pzn7vQ6O6/yqcKayXUSpQJ2n6BGRV9
4e9R/oWh5r6dl5ZEck6uQcOIwWRf+xbLrjF/BwdsGxSc+HhnNoGv2zPQn5CDY8Lg
sTAUKNrs1+owqrINKQVoR5A2o6CmBdXCwZbS1l6RlWAMPo2N6c1FVqPGDVYu1Tdk
esOiKT6SkeF2hHFUTWgGg+3LnDi81soYZQIDAQAB
-----END RSA PUBLIC KEY-----
```

#### openssl

```
$ openssl genrsa -out key
Generating RSA private key, 2048 bit long modulus
...........................................................................................................................................................+++
................................................+++
e is 65537 (0x10001)
$ openssl rsa -in key -out key.pub -pubout
writing RSA key
$ ls
key  key.pub
$ cat key
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAywDkOcJZKWjo8DJGut27ziNpAhjlnzXj/pXJaNbZyl2y4W4w
inpjllb9sRpJwLOb1za0fLs8AlwNWf6479JmBZFG8J5cs7f3W0hnsycTZC3wTgn8
KEy5z7LzDGTDsQE2s1bKa8cmgrewTPiWX/lE2io2Zugn7oke+rOAwmlbFnXUM8jc
NG4wi8c/lDmlEhHT+ftwYkmCcoI64iyn8kkxb1jU3bpwznoq95NtfVoGDiw2b5Xz
Do4zSiTVmRMHbCSC3UudqkLaJ+Fq1aWcLJmf4F7OjJD3D/IHCiE/o6ujQ5mw/rvx
gg2LVPVFI7OKceATfl4om02TpLmW14cGMKxgswIDAQABAoIBAQClPDSITwkEC8CO
wl2P/KvBPlFaw6kjzlml/4XRMBBWB65yj54zGSu2U1hrr1oY4BkZNA30+PH/ea3k
ctFVY4xnO7jiYgRSxIsyYd0GlefSwxG2c6oro0gbJy1uCRbAHJzJnPtmftN50spN
3ImJ1PuWGIXvfUgDgNkWdsV3eQoLXKnVygw9tVM/as+B7QWM/GE5NZ5aewghAkXB
4muT0fwZvGFGFDWwy2u9SVx5Ga6o5s3RVPVVi6icq1c0lJbXpSeAcLXPxxgoWErl
w8aWGMd5HNuGW+S8Wh10QSMcQebYFeMeiNC/f5BKJFiJ7sUs4g0l8D4pybjHtJyj
gelQ5kQBAoGBAO0k79LHdb6+Dt9Iw2O9J2CGMeHoDo0OxiG0BVThK/ByHUSzCnbl
UqYrw/3enRPLo5C2VPpV0LG86ei26n+zYBZp0ZdraAF8NbEsmGBxXA29PzxFMoyZ
KNwSiH7kNv14FW19Ed1an3MXWbxhYqQEu4ccKFVRRjq/wecbkQjkYdqxAoGBANsl
BRn5U0a0Y/63l2Pp3meGGyRv+RCXIWvvJqVF9Cypa4AvCa5UpcTyQULEGkd6e8rF
hqj4AYvNBO1YMk3EdS+cXQvs3RygsD1un/x9gjpc/4eqxPJo+Ffk3jUqeCozWF2r
OXBoxbT9AmmrZix5jBUs7ynuT1vh1PV+FFWp5MKjAoGAK2v3/cBH7wUd9AR+wmSA
MUxhFdxHzcxGuyUbVM9mHEDI2yttXxfeak7ZNELuwBEpgfXg2XWdIO5WZiYYCgnu
LixyFCe2g48axbZEsGQRVvKwAXkqKFRjBujd1V40M3Upd0LPYpiVWZ2M8NFkHmQ/
ch3VxMl88oTqHEcP53Dp+gECgYEAjNJzPserLepJv6s2i6wDkT7qPjX+NR0JCxT5
HoefavvL6ulPLUmT5MVQDpl50ajx19GqwPXt+Gpc2jAs0zCFpXyzhjBFvlm8Jx3p
jRUSFHMieYnRWXz+6WxXJo8N/QBVrouTM5sAQVe8ry7HT5kj3Y+seKN9N6wdb0v/
wTSVnkUCgYEAj9e2Hs+jxeRV2qV43p4kmpsRgWpc5xDZ4JFlc6P7gxGIDn146oFZ
KDE1wt6xkwuxsiSHe+fPdZsgH6ub++Zxtu3E9825DlTh6Ocu7DopIeMmYxmz5AQV
fEST9tUNBEXBiIG1UuJ3fqb8t2GrBfQIrjZhjWfY2u0qS7gP18/QiwM=
-----END RSA PRIVATE KEY-----
$ cat key.pub
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAywDkOcJZKWjo8DJGut27
ziNpAhjlnzXj/pXJaNbZyl2y4W4winpjllb9sRpJwLOb1za0fLs8AlwNWf6479Jm
BZFG8J5cs7f3W0hnsycTZC3wTgn8KEy5z7LzDGTDsQE2s1bKa8cmgrewTPiWX/lE
2io2Zugn7oke+rOAwmlbFnXUM8jcNG4wi8c/lDmlEhHT+ftwYkmCcoI64iyn8kkx
b1jU3bpwznoq95NtfVoGDiw2b5XzDo4zSiTVmRMHbCSC3UudqkLaJ+Fq1aWcLJmf
4F7OjJD3D/IHCiE/o6ujQ5mw/rvxgg2LVPVFI7OKceATfl4om02TpLmW14cGMKxg
swIDAQAB
-----END PUBLIC KEY-----
```

`key`와 `key.pub`은 OpenSSL에서 사용할 수 있는 개인/공개 키다.

ssh-keygen으로 생성한 공개 키와 openssl로 생성한 공개 키 형식이 다르다.  ssh-keygen으로 생성한 건 PKCS#1 표준 파일, openssl로 생성한 건 x.509 표준 파일이다. 나중에 API로 공개 키를 불러올 때 구분해줘야 한다.

openssl로 PKCS#1 표준 파일을 만들려면 아래처럼 한다.

```
$ openssl rsa -in key -out key.pub -RSAPublicKey_out
writing RSA key
$ cat key.pub
-----BEGIN RSA PUBLIC KEY-----
MIIBCgKCAQEAnTJkP788filk1I5vtOaLTWmJASMOIZBL/D9inIlPH7oLxGpuIfn8
XUNXOQwJDegUkh5Eo3iI1aC27ImTT65X0TDDtB9VWXpLbfm6kDp/pG3DG5DRbb5m
0fgBopoDfjoN3zJYiXD0ztYZgy8+Iq5iVufIiJuHfWREkzq3/ega7SyPE/Lya8t4
ZgLfJoUHJBf9T3jNc4+b6krYFsj6bU/MkDIn3+cLkbrQvtpcdTeXn7A1fVa06h9L
/YfRonr8wAKl5Smtm33V5I5/3A0lA6SMF+GjbazjDAAwwOsFMaOgwX45ENSBw/No
JdT7KakhGGeaAhlh9DTWPoffSa6ZxHxncQIDAQAB
-----END RSA PUBLIC KEY-----
```

### PEM

사생활 보호 전자 메일(Privacy-enhanced Electronic Mail)

원래 용도로는 많이 사용하지 않다가 암호 키, 인증서 등등을 저장하는 용도로 많이 쓴다.

앞서 키 쌍을 만들 때 두 가지 방법으로 만들면 공개 키가 다르게 만들어진다. 각각 PKCS#1 표준과 x.509 표준을 따른다.

#### PKCS#1

```
-----BEGIN RSA PUBLIC KEY-----
...
-----END RSA PUBLIC KEY-----
```

#### x.509

```
-----BEGIN PUBLIC KEY-----
...
-----END PUBLIC KEY-----
```


## 암호화

- 공개 키를 불러온다.

  - PEM_read_RSAPublicKey
    - PKCS#1 표준 PEM 파일을 읽는다.
  - PEM_read_RSA_PUBKEY
    - x.509 표준 PEM 파일을 읽는다.

- 암호화한다.

  - RSA_public_encrypt


## 복호화

- 개인 키를 불러온다.
  - PEM_read_RSAPrivateKey
- 복호화한다.
  - RSA_private_decrypt


## 계수

키를 만들 때 계수를 설정한다. 기본은 2048이다. 계수에 따라 암호화된 블럭 크기가 달라진다. 블럭 크기에 따라 암호화할 수 있는 데이터 크기가 달라진다. 아래 함수는 해당 키의 블럭 크기를 계산해 반환한다.

- RSA_size

## 여유 공간

암호화할 데이터 크기가 블럭 크기보다 작다면 남은 공간을 채워 블럭 크기에 맞춘다. 이 과정이 없다면 안전하지 않다.

- RSA_PKCS1_PADDING

  가장 널리 쓰이지만 보안 허점이 발견됐다. 데이터 크기가 RSA_size - 11보다 작아야 한다.

- RSA_PKCS1_OAEP_PADDING

  가장 추천한다. 데이터 크기가 RSA_size - 41보다 작아야 한다.

- RSA_SSLV23_PADDING

  SSL 표준에 쓴다.

- RSA_NO_PADDING

  여유 공간을 만들지 않는다. 안전하지 않다. 데이터 크기가 정확히 RSA_size와 같아야 한다.

# 참고

[DigitalOcean - OpenSSL Essentials: Working with SSL Certificates, Private Keys and CSRs](https://www.digitalocean.com/community/tutorials/openssl-essentials-working-with-ssl-certificates-private-keys-and-csrs)

[IBM Knowledge Center - Cryptographic security protocols: SSL and TLS](https://www.ibm.com/support/knowledgecenter/en/SSFKSJ_7.1.0/com.ibm.mq.doc/sy10630_.htm)

[OpenSSL - crypto library](https://www.openssl.org/docs/manmaster/man3/)