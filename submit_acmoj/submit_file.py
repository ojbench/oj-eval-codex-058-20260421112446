#!/usr/bin/env python3
import argparse, os, json, requests

def main():
    ap = argparse.ArgumentParser(description='Submit code file to ACMOJ')
    ap.add_argument('--token', default=os.environ.get('ACMOJ_TOKEN'))
    ap.add_argument('--problem-id', type=int, required=True)
    ap.add_argument('--file', dest='file_path', required=True)
    ap.add_argument('--language', default='c++')
    args = ap.parse_args()
    if not args.token:
        print('Error: token missing'); return 1
    try:
        with open(args.file_path,'r',encoding='utf-8') as f:
            code = f.read()
    except Exception as e:
        print('Read file failed:', e); return 1
    url = fhttps://acm.sjtu.edu.cn/OnlineJudge/api/v1/problem/{args.problem_id}/submit
    headers = {
        'Authorization': f'Bearer {args.token}',
        'Content-Type': 'application/x-www-form-urlencoded',
        'User-Agent': 'ACMOJ-Python-Client/2.2'
    }
    data = {'language': args.language, 'code': code}
    try:
        r = requests.post(url, headers=headers, data=data, timeout=15, proxies={'https': None, 'http': None})
        r.raise_for_status()
        resp = r.json()
        print(json.dumps(resp))
    except Exception as e:
        print('Submit failed:', e)
        try:
            print('Response:', r.text)
        except Exception:
            pass
        return 1

if __name__ == '__main__':
    raise SystemExit(main())
