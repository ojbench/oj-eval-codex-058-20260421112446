#!/usr/bin/env python3
import os, argparse, json
from acmoj_client import ACMOJClient

def main():
    ap = argparse.ArgumentParser(description='Submit src.hpp content to ACMOJ')
    ap.add_argument('--token', default=os.environ.get('ACMOJ_TOKEN'))
    ap.add_argument('--problem-id', type=int, required=True)
    ap.add_argument('--file', default='src.hpp')
    ap.add_argument('--language', default='c++')
    args = ap.parse_args()
    if not args.token:
        print('Error: Access token not provided. Use --token or set ACMOJ_TOKEN')
        return 1
    try:
        with open(args.file, 'r', encoding='utf-8') as f:
            code = f.read()
    except Exception as e:
        print('Failed to read file:', e)
        return 1
    client = ACMOJClient(args.token)
    result = client._make_request('POST', f/problem/{args.problem_id}/submit, data={'language': args.language, 'code': code})
    if result:
        print(json.dumps(result))
        return 0
    return 1

if __name__ == '__main__':
    raise SystemExit(main())

