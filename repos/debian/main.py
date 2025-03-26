import re
import random
import time
import datetime

class ElizaAI:
    def __init__(self):
        # 定义关键词和对应的回复模板。(.*)指任意消息；括号外为固定关键词；括号内为可触发的关键词，使用|分隔；$表示到此为止后面，如果出现其他内容则不触发；{0}表示第一个括号的内容，{1}表示第二个，以此类推。
        self.patterns = [
            (r'(.*)我(需要|想要|想|要|希望)(.*)', [
                "你为什么{1}{2}？",
                "{2}对你来说意味着什么呢",
                "你真的{1}{2}吗？"
            ]),
            (r'(.*)吗$', [
                "{0}",
            ]),

            (r'(.*)', [
                "啊？",
                "服务器繁忙，请稍后再试。",
                "嗯。",
                "{0}？",
                "什么意思"
            ])
        ]

        # 用于记录用户的历史输入
        self.history = []
        # 用于存储重复问题的回复
        self.repeat_responses = [
            "你一直在重复，是在测试我吗？",
            "转人工",
            "你是复读机吧"
        ]
        # 时间查询的正则表达式
        self.time_query_patterns = [
            r'.*(几点|时间|现在时间).*',
            r'.*(现在是|当前时间).*',
            r'.*(告诉我时间|告诉我现在几点).*'
        ]

    def respond(self, user_input):
        """根据用户输入生成回复"""
        # 记录用户输入
        self.history.append(user_input)

        # 检查重复问题
        if len(self.history) >= 3 and self.history[-1] == self.history[-2] == self.history[-3]:
            return random.choice(self.repeat_responses)

        # 时间查询
        for pattern in self.time_query_patterns:
            if re.match(pattern, user_input, re.IGNORECASE):
                now = datetime.datetime.now()
                return f"现在是{now.year}年{now.month}月{now.day}日{now.hour}:{now.minute}:{now.second}"

        # 匹配用户输入与模式，并生成回复
        for pattern, responses in self.patterns:
            match = re.match(pattern, user_input.lower())
            if match:
                response_template = random.choice(responses)
                response = self._format_response(response_template, match)
                return response

        return "服务器繁忙，请稍后再试。"

    def _format_response(self, template, match):
        if not match.groups():
            return template.replace('{0}', '').replace('{1}', '')

        try:
            groups = match.groups()
            # 代词转换
            converted_groups = [self._switch_pronouns(group) for group in groups]
            result = template.format(*converted_groups)
            return result
        except IndexError:
            return template.replace('{0}', '').replace('{1}', '')

    def _switch_pronouns(self, text):
        """代词转换"""
        return text.replace('你', '%TEMP%').replace('我', '你').replace('%TEMP%', '我')

    def stream_output(self, text, prefix="", delay=0.05):
        """流式输出（上面delay参数是出字间隔，单位为秒，不需要流式请归零）"""
        if prefix:
            print(prefix, end="", flush=True)
        try:
            for char in text:
                print(char, end='', flush=True)
                time.sleep(delay)
            print()
        except Exception as e:
            print(prefix + text)
            print(f"注意：流式输出失败，原因：{e}")

def main():
    """主函数，运行Eliza"""
    print("你好，我是Eliza！")
    Eliza = ElizaAI()

    while True:
        try:
            user_input = input("用户：")
            if user_input.lower() in ['再见', '退出', '拜拜']:
                Eliza.stream_output("拜拜")
                break

            response = Eliza.respond(user_input)
            Eliza.stream_output(response, prefix="Eliza：")
        except KeyboardInterrupt:
            print("\n程序被用户中断。")
            break
        except Exception as e:
            print(f"发生错误：{e}")
            break

if __name__ == "__main__":
    main()
